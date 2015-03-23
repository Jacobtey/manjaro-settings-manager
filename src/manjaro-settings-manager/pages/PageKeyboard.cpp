/*
 *  Manjaro Settings Manager
 *  Roland Singer <roland@manjaro.org>
 *  Ramon Buldó <ramon@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PageKeyboard.h"
#include "ui_PageKeyboard.h"
#include "SetKeyboardLayoutJob.h"

#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QMapIterator>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtWidgets/QMessageBox>

#include <QDebug>

PageKeyboard::PageKeyboard(QWidget *parent) :
    PageWidget(parent),
    ui(new Ui::PageKeyboard),
    keyboardModel_(new KeyboardModel),
    keyboardProxyModel_(new QSortFilterProxyModel),
    keyboardPreview_(new KeyBoardPreview)
{
    ui->setupUi(this);
    setTitel(tr("Keyboard Settings"));
    setIcon(QPixmap(":/images/resources/keyboard.png"));
    setShowApplyButton(true);

    // Keyboard preview widget
    ui->KBPreviewLayout->addWidget(keyboardPreview_);
    keyboardPreview_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connect signals and slots
    connect(ui->buttonRestore, &QPushButton::clicked,
            this, &PageKeyboard::buttonRestore_clicked);
    connect(ui->layoutsListView, &QListView::clicked,
            this, &PageKeyboard::setDefaultIndexToVariantListView);
    connect(ui->variantsListView, &QListView::clicked,
            this, &PageKeyboard::setKeyboardPreviewLayout);


    keyboardProxyModel_->setSourceModel(keyboardModel_);
    keyboardProxyModel_->setSortLocaleAware(true);
    keyboardProxyModel_->setSortRole(KeyboardModel::DescriptionRole);
    keyboardProxyModel_->sort(0, Qt::AscendingOrder);
    ui->layoutsListView->setModel(keyboardProxyModel_);

    // Find root layout index and set it in the layoutsRootView
    QModelIndexList layoutsRootList = keyboardProxyModel_->match(keyboardProxyModel_->index(0,0),
                                                                KeyboardModel::KeyRole,
                                                                "layouts",
                                                                Qt::MatchFixedString);
    if (layoutsRootList.size() == 1) {
        QModelIndex layoutsRoot = layoutsRootList.first();
        ui->layoutsListView->setRootIndex(layoutsRoot);
    } else {
        qDebug() << "Can't find keyboard layout list";
    }

    layoutsSelectionProxy_ = new KSelectionProxyModel(ui->layoutsListView->selectionModel(), this);
    layoutsSelectionProxy_->setSourceModel(keyboardModel_);
    layoutsSelectionProxy_->setFilterBehavior(KSelectionProxyModel::ChildrenOfExactSelection);

    variantsSortProxy_ = new QSortFilterProxyModel();
    variantsSortProxy_->setSourceModel(layoutsSelectionProxy_);
    variantsSortProxy_->setSortLocaleAware(true);
    variantsSortProxy_->sort(0, Qt::AscendingOrder);
    ui->variantsListView->setModel(variantsSortProxy_);

    // Set root index to the model combo box
    ui->modelComboBox->setModel(keyboardProxyModel_);
    QModelIndexList modelsRootList = keyboardProxyModel_->match(keyboardProxyModel_->index(0,0),
                                                       KeyboardModel::KeyRole,
                                                       "models", Qt::MatchFixedString);
    if (modelsRootList.size() == 1) {
        QModelIndex modelsRoot = modelsRootList.first();
        ui->modelComboBox->setRootModelIndex(modelsRoot);
    } else {
        qDebug() << "Can't find keyboard model list";
    }
}


PageKeyboard::~PageKeyboard()
{
    delete ui;
    delete keyboardModel_;
    delete keyboardProxyModel_;
    delete keyboardPreview_;
    delete layoutsSelectionProxy_;
    delete variantsSortProxy_;
}


void PageKeyboard::apply_clicked()
{
    setKeyboardLayout();
}


void PageKeyboard::setKeyboardLayout()
{
    QString model = ui->modelComboBox->itemData(ui->modelComboBox->currentIndex(), KeyboardModel::KeyRole).toString();
    QString layout = ui->layoutsListView->currentIndex().data(KeyboardModel::KeyRole).toString();
    QString variant = ui->variantsListView->currentIndex().data(KeyboardModel::KeyRole).toString();

    if (QString::compare(variant, "default") == 0) {
        variant = "";
    }

    // Set Xorg keyboard layout
    system(QString("setxkbmap -model \"%1\" -layout \"%2\" -variant \"%3\"").arg(model, layout, variant).toUtf8());

    bool error = false;

    bool isKeyboardctlInstalled = QFile::exists("/usr/bin/keyboardctl");
    if (isKeyboardctlInstalled) {
        ApplyDialog dialog(this);
        dialog.exec("keyboardctl", QStringList() << "--set-layout" << model << layout << variant, tr("Setting new keyboard layout..."), true);
        if (!dialog.processSuccess()) {
            error = true;
        }
    } else {
        // remove leftover keyboardctl file
        const QString keyboardctlFile("/etc/X11/xorg.conf.d/20-keyboard.conf");
        if (QFile::exists(keyboardctlFile)) {
            QFile::remove(keyboardctlFile);
        }
    }

    // localed d-bus interface to set X11 keyboard layout
    QDBusInterface dbusInterface("org.freedesktop.locale1",
                                 "/org/freedesktop/locale1",
                                 "org.freedesktop.locale1",
                                 QDBusConnection::systemBus());
    QVariant optionsVariant = dbusInterface.property("X11Options");

    if (optionsVariant.isValid()) {
         QString options = optionsVariant.toString();
        /* ssssbb
             * string -> layout
             * string -> model
             * string -> variant
             * string -> options
             * boolean -> convert (set vconsole keyboard too)
             * boolean -> arg_ask_password
             */
        QDBusMessage reply;
        reply = dbusInterface.call("SetX11Keyboard", layout, model, variant, options, true, true);
        if (reply.type() == QDBusMessage::ErrorMessage) {
            error = true;
        }
    } else {
        SetKeyboardLayoutJob job(model, layout, variant);
        if (job.exec() == false) {
            error = true;
        }
    }

    if (error) {
        QMessageBox::warning(this,
                             tr("Error!"),
                             QString(tr("Failed to set keyboard layout")),
                             QMessageBox::Ok, QMessageBox::Ok);
    } else {
        currentLayout_ = layout;
        currentVariant_ = variant;
        currentModel_ = model;
    }
}


void PageKeyboard::activated()
{
    // Default focus
    ui->layoutsListView->setFocus();

    // Detect current keyboard layout, variant and model
    if (!keyboardModel_->getCurrentKeyboardLayout(currentLayout_, currentVariant_, currentModel_)) {
       qDebug() << "Failed to determine current keyboard layout";
    }

    if (currentLayout_.isEmpty()) {
        currentLayout_ = "us";
    }
    if (currentVariant_.isEmpty()) {
        currentVariant_ = "default";
    }
    if (currentModel_.isEmpty()) {
        currentModel_ = "pc105";
    }

    setLayoutsListViewIndex(currentLayout_);
    setVariantsListViewIndex(currentVariant_);
    setModelComboBoxIndex(currentModel_);
}


void PageKeyboard::setLayoutsListViewIndex(const QString &layout)
{
    QModelIndexList layoutIndexList = keyboardProxyModel_->match(ui->layoutsListView->rootIndex().child(0,0),
                                                                 KeyboardModel::KeyRole,
                                                                 layout,
                                                                 Qt::MatchFixedString);

    if (layoutIndexList.size() == 1) {
        QModelIndex layoutIndex = layoutIndexList.first();
        ui->layoutsListView->setCurrentIndex(layoutIndex);
    } else {
        qDebug() << QString("Can't find the keyboard layout %1").arg(layout);
    }
}


void PageKeyboard::setVariantsListViewIndex(const QString &variant)
{
    QAbstractItemModel *model = ui->variantsListView->model();
    QModelIndexList variantDefaultList = model->match(model->index(0,0),
                                                      KeyboardModel::KeyRole,
                                                      variant,
                                                      Qt::MatchFixedString);
    if (variantDefaultList.size() == 1) {
        QModelIndex variantDefault = variantDefaultList.first();
        ui->variantsListView->setCurrentIndex(variantDefault);
        // Emit clicked(), to update keyboardPreview
        emit(ui->variantsListView->clicked(variantDefault));
    } else {
        qDebug() << QString("Can't find the keyboard variant %1").arg(variant);
    }
}


void PageKeyboard::setModelComboBoxIndex(const QString &model)
{
    QModelIndexList modelIndexList = keyboardProxyModel_->match(ui->modelComboBox->rootModelIndex().child(0,0),
                                                                KeyboardModel::KeyRole,
                                                                model,
                                                                Qt::MatchFixedString);
    if (modelIndexList.size() == 1) {
        QModelIndex modelIndex = modelIndexList.first();
        ui->modelComboBox->setCurrentIndex(modelIndex.row());
    } else {
        qDebug() << QString("Can't find the keyboard model %1").arg(model);
    }
}


void PageKeyboard::buttonRestore_clicked()
{
    setLayoutsListViewIndex(currentLayout_);
    setVariantsListViewIndex(currentVariant_);
    setModelComboBoxIndex(currentModel_);
}


void PageKeyboard::setDefaultIndexToVariantListView(const QModelIndex &index)
{
    if (index.isValid()) {
        setVariantsListViewIndex("default");
    }
}


void PageKeyboard::setKeyboardPreviewLayout(const QModelIndex &index)
{
    if (index.isValid()) {
        QString layout = ui->layoutsListView->currentIndex().data(KeyboardModel::KeyRole).toString();
        QString variant = ui->variantsListView->currentIndex().data(KeyboardModel::KeyRole).toString();
        keyboardPreview_->setLayout(layout);
        keyboardPreview_->setVariant(variant);
    }
}

#-------------------------------------------------
#
# Project created by QtCreator 2013-03-13T01:28:06
#
#-------------------------------------------------

QT += core widgets dbus KItemModels

LIBS += -lhd -licudata -licuuc
TARGET = manjaro-settings-manager-gui
TEMPLATE = app
CONFIG += c++11

SOURCES  += main.cpp \
            mainwindow.cpp \
            delegates/KernelListViewDelegate.cpp \
            delegates/LanguageListViewDelegate.cpp \
            dialogs/accounttypedialog.cpp \
            dialogs/adduserdialog.cpp \
            dialogs/applydialog.cpp \
            dialogs/changepassworddialog.cpp \
            dialogs/previewfiledialog.cpp \
            dialogs/KernelInfoDialog.cpp \
            dialogs/SelectLocalesDialog.cpp \
            dialogs/TimeZoneDialog.cpp \
            mhwd/mhwd.cpp \
            mhwd/vita/string.cpp \
            models/EnabledLocalesModel.cpp \
            models/KernelModel.cpp \
            models/Kernel.cpp \
            models/KeyboardItem.cpp \
            models/KeyboardModel.cpp \
            models/SupportedLocalesModel.cpp \
            models/SupportedLocalesItem.cpp \
            models/TimeDate.cpp \
            pages/PageKernel.cpp \
            pages/PageKeyboard.cpp \
            pages/PageLanguage.cpp \
            pages/PageLanguagePackages.cpp \
            pages/PageMhwd.cpp \
            pages/PageNotifications.cpp \
            pages/PageTimeDate.cpp \
            pages/PageUsers.cpp \
            pages/SetKeyboardLayoutJob.cpp \
            widgets/FlatButton.cpp \
            widgets/ListWidget.cpp \
            widgets/LocaleGlobal.cpp \
            widgets/keyboardpreview.cpp \
            widgets/PageWidget.cpp \
            widgets/TimeZoneWidget.cpp




HEADERS  += mainwindow.h \
            delegates/KernelListViewDelegate.h \
            delegates/LanguageListViewDelegate.h \
            dialogs/accounttypedialog.h \
            dialogs/adduserdialog.h \
            dialogs/applydialog.h \
            dialogs/changepassworddialog.h \
            dialogs/previewfiledialog.h \
            dialogs/KernelInfoDialog.h \
            dialogs/SelectLocalesDialog.h \
            dialogs/TimeZoneDialog.h \
            mhwd/const.hpp \
            mhwd/mhwd.hpp \
            mhwd/mhwd_p.hpp \
            mhwd/vita/string.hpp \
            models/EnabledLocalesModel.h \
            models/KernelModel.h \
            models/Kernel.h \
            models/KeyboardItem.h \
            models/KeyboardModel.h \
            models/SupportedLocalesModel.h \
            models/SupportedLocalesItem.h \
            models/TimeDate.h \
            pages/PageKernel.h \
            pages/PageKeyboard.h \
            pages/PageLanguage.h \
            pages/PageLanguagePackages.h \
            pages/PageMhwd.h \
            pages/PageNotifications.h \
            pages/PageTimeDate.h \
            pages/PageUsers.h \
            pages/SetKeyboardLayoutJob.h \
            widgets/FlatButton.h \
            widgets/ListWidget.h \
            widgets/LocaleGlobal.h \
            widgets/keyboardpreview.h \
            widgets/PageWidget.h \
            widgets/TimeZoneWidget.h




FORMS    += mainwindow.ui \
            dialogs/adduserdialog.ui \
            dialogs/accounttypedialog.ui \
            dialogs/applydialog.ui \
            dialogs/changepassworddialog.ui \
            dialogs/SelectLocalesDialog.ui \
            dialogs/TimeZoneDialog.ui \
            pages/PageKernel.ui \
            pages/PageKeyboard.ui \
            pages/PageLanguage.ui \
            pages/PageLanguagePackages.ui \
            pages/PageMhwd.ui \
            pages/PageNotifications.ui \
            pages/PageTimeDate.ui \
            pages/PageUsers.ui




TRANSLATIONS += translations/msm_ar.ts \
                translations/msm_ast.ts \
                translations/msm_az_AZ.ts \
                translations/msm_bg_BG.ts \
                translations/msm_ca.ts \
                translations/msm_cs_CZ.ts \
                translations/msm_da.ts \
                translations/msm_de.ts \
                translations/msm_el.ts \
                translations/msm_en.ts \
                translations/msm_es.ts \
                translations/msm_fa.ts \
                translations/msm_fa_IR.ts \
                translations/msm_fi.ts \
                translations/msm_fo.ts \
                translations/msm_fr.ts \
                translations/msm_gl.ts \
                translations/msm_gl_ES.ts \
                translations/msm_he.ts \
                translations/msm_hi.ts \
                translations/msm_hr.ts \
                translations/msm_hu.ts \
                translations/msm_id_ID.ts \
                translations/msm_is.ts \
                translations/msm_it.ts \
                translations/msm_ja.ts \
                translations/msm_ka.ts \
                translations/msm_lt.ts \
                translations/msm_ml.ts \
                translations/msm_ms.ts \
                translations/msm_nb.ts \
                translations/msm_nl.ts \
                translations/msm_pl.ts \
                translations/msm_pt_BR.ts \
                translations/msm_pt_PT.ts \
                translations/msm_ro.ts \
                translations/msm_ru.ts \
                translations/msm_sk_SK.ts \
                translations/msm_sl.ts \
                translations/msm_sr.ts \
                translations/msm_sr@latin.ts \
                translations/msm_sr_RS.ts \
                translations/msm_sr_RS@latin.ts \
                translations/msm_sv.ts \
                translations/msm_th_TH.ts \
                translations/msm_tr.ts \
                translations/msm_uk.ts \
                translations/msm_ur_PK.ts \
                translations/msm_uz@Latn.ts \
                translations/msm_vi.ts \
                translations/msm_zh_CN.ts \
                translations/msm_zh_TW.ts

RESOURCES += \
    resources.qrc

unix:!macx: LIBS += -L$$OUT_PWD/../global/ -lglobal

INCLUDEPATH += $$PWD/../global
DEPENDPATH += $$PWD/../global

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../global/libglobal.a

target.path = /usr/bin
INSTALLS += target

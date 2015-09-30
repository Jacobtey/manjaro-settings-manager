#-------------------------------------------------
#
# Project created by QtCreator 2013-03-13T01:28:06
#
#-------------------------------------------------

QT       += core gui

TARGET = manjaro-settings-manager-daemon
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
    daemon.cpp \
    models/Kernel.cpp \
    models/KernelModel.cpp

HEADERS  += daemon.h \
    models/Kernel.h \
    models/KernelModel.h

unix:!macx: LIBS += -L$$OUT_PWD/../global/ -lglobal

INCLUDEPATH += $$PWD/../global
DEPENDPATH += $$PWD/../global

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../global/libglobal.a

TRANSLATIONS += translations/msmd_ar.ts \
                translations/msmd_ast.ts \
                translations/msmd_az_AZ.ts \
                translations/msmd_bg_BG.ts \
                translations/msmd_ca.ts \
                translations/msmd_cs_CZ.ts \
                translations/msmd_da.ts \
                translations/msmd_de.ts \
                translations/msmd_el.ts \
                translations/msmd_en.ts \
                translations/msmd_es.ts \
                translations/msmd_fa.ts \
                translations/msmd_fa_IR.ts \
                translations/msmd_fi.ts \
                translations/msmd_fo.ts \
                translations/msmd_fr.ts \
                translations/msmd_he.ts \
                translations/msmd_hi.ts \
                translations/msmd_hr.ts \
                translations/msmd_hu.ts \
                translations/msmd_id_ID.ts \
                translations/msmd_is.ts \
                translations/msmd_it.ts \
                translations/msmd_ja.ts \
                translations/msmd_ka.ts \
                translations/msmd_lt.ts \
                translations/msmd_ml.ts \
                translations/msmd_ms.ts \
                translations/msmd_nb.ts \
                translations/msmd_nl.ts \
                translations/msmd_pl.ts \
                translations/msmd_pt_BR.ts \
                translations/msmd_pt_PT.ts \
                translations/msmd_ro.ts \
                translations/msmd_ru.ts \
                translations/msmd_sk_SK.ts \
                translations/msmd_sl.ts \
                translations/msmd_sr.ts \
                translations/msmd_sr@latin.ts \
                translations/msmd_sr_RS.ts \
                translations/msmd_sr_RS@latin.ts \
                translations/msmd_sv.ts \
                translations/msmd_th_TH.ts \
                translations/msmd_tr.ts \
                translations/msmd_uk.ts \
                translations/msmd_ur_PK.ts \
                translations/msmd_uz@Latn.ts \
                translations/msmd_vi.ts \
                translations/msmd_zh_CN.ts \
                translations/msmd_zh_TW.ts

RESOURCES += \
    resources.qrc


target.path = /usr/bin
INSTALLS += target



include(qtsingleapplication/src/qtsingleapplication.pri)

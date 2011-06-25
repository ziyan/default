QT += network opengl sql
CONFIG += no_keywords debug
TARGET = slam
TEMPLATE = app
QMAKE_CLEAN += */*~

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui

INCLUDEPATH += /usr/local/include/player-3.0
LIBS += -lplayerc++
DEPENDPATH += ui/ model/ utils/

SOURCES += main.cpp \
	ui/glwidget.cpp \
	model/robot.cpp model/world.cpp model/map.cpp
HEADERS += ui/glwidget.h \
	model/robot.h model/world.h model/map.h
FORMS +=

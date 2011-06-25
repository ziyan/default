QT += gui core network opengl sql
CONFIG += no_keywords debug
TARGET = 
TEMPLATE = app
QMAKE_CLEAN += */*~

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
CUDA_DIR = $$system(which nvcc | sed 's,/bin/nvcc$,,')

INCLUDEPATH += /usr/include/player-3.0
INCLUDEPATH += $$CUDA_DIR/include

QMAKE_LIBDIR += $$CUDA_DIR/lib64
LIBS += -lplayerc++
LIBS += -lcudart

DEPENDPATH += ui/ model/ utils/

SOURCES += main.cpp \
        ui/glwidget.cpp \
        model/robot.cpp model/world.cpp model/map.cpp
HEADERS += ui/glwidget.h \
        model/robot.h model/world.h model/map.h
FORMS +=
CUDAS += cuda/map.cu

# CUDA compiler
# -deviceemu -c -Xcompiler $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,' -I ','-I ')
cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}.cuda.o
cuda.commands = nvcc -c -Xcompiler $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,' -I ','-I ') ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
cuda.dependcy_type = TYPE_C
cuda.depend_command = nvcc -M -Xcompiler $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,' -I ','-I ') ${QMAKE_FILE_NAME} | sed "s,^.*: ,," | sed "s,^ *,," | tr -d '\\\n'

cuda.input = CUDAS
QMAKE_EXTRA_UNIX_COMPILERS += cuda

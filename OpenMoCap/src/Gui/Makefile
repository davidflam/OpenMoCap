# OpenMocap
# Executes Qt's Meta-Object Compiler, generating .moc files

MAKE_BIN =mingw32-make.exe -j4
QT_MOC_COMPILER = moc
MOC_OBJECTS = $(patsubst %.h, %.moc, $(wildcard *.h))

all:
	$(MAKE_BIN) clean -f Makefile -C .
	$(MAKE_BIN) mocs -f Makefile -C .
	$(MAKE_BIN) clean -f ../Makefile -C Dialogs
	$(MAKE_BIN) mocs -f ../Makefile -C Dialogs
	$(MAKE_BIN) clean -f ../Makefile -C Widgets
	$(MAKE_BIN) mocs -f ../Makefile -C Widgets
	
clean: $(MOC_OBJECTS)
	del *.moc

mocs: $(MOC_OBJECTS)

%.moc: %.h
	$(QT_MOC_COMPILER) $< -o $@
	

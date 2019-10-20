CC = cl.exe /EHsc /MT /c
LN = link.exe
EXENAME = ChatServer

all: $(EXENAME).exe
	@echo BUILD SUCCESSFUL
$(EXENAME).exe: $(EXENAME).obj 
	$(LN) -out:$(EXENAME).exe *.obj RakNetLibStatic.lib WSock32.lib
	rm *.obj
$(EXENAME).obj: src/$(EXENAME).cpp
	$(CC) -Fo$(EXENAME).obj src/$(EXENAME).cpp
clean:
	rm *.exe
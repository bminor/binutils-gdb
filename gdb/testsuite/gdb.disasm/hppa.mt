EXECUTABLES = hppa
hppa: hppa.s
	$(AS) $(ASFLAGS) hppa.s -o hppa.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o hppa hppa.o $(LIBS)


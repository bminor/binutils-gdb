# Target: WDC 65816 simulator
TDEPFILES=  w65-tdep.o remote-sim.o libsim.a

other:all

libsim.a:
	cp ../sim/w65/libsim.a .
	ranlib libsim.a


TM_FILE= tm-w65.h

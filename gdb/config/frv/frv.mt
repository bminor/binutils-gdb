# Target: Fujitsu FRV processor
TDEPFILES= frv-tdep.o frv-linux-tdep.o solib.o solib-frv.o corelow.o
SIM_OBS = remote-sim.o
SIM = ../sim/frv/libsim.a

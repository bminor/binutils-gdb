	.syntax unified
	.text
	.arch armv8-a
	.arch_extension fp

	.arm
	vseleq.f32	s0, s0, s0
	vselvs.f32	s1, s1, s1
	vselge.f32	s30, s30, s30
	vselgt.f32	s31, s31, s31
	vseleq.f64	d0, d0, d0
	vselvs.f64	d16, d16, d16
	vselge.f64	d15, d15, d15
	vselgt.f64	d31, d31, d31

	.thumb
	vseleq.f32	s0, s0, s0
	vselvs.f32	s1, s1, s1
	vselge.f32	s30, s30, s30
	vselgt.f32	s31, s31, s31
	vseleq.f64	d0, d0, d0
	vselvs.f64	d16, d16, d16
	vselge.f64	d15, d15, d15
	vselgt.f64	d31, d31, d31

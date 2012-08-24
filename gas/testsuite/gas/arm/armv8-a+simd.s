	.syntax unified
	.arch armv8-a
	.arch_extension simd

	.arm
	vmaxnm.f32	d0, d0, d0
	vmaxnm.f32	d16, d16, d16
	vmaxnm.f32	d15, d15, d15
	vmaxnm.f32	d31, d31, d31
	vmaxnm.f32	q0, q0, q0
	vmaxnm.f32	q8, q8, q8
	vmaxnm.f32	q7, q7, q7
	vmaxnm.f32	q15, q15, q15
	vminnm.f32	d0, d0, d0
	vminnm.f32	d16, d16, d16
	vminnm.f32	d15, d15, d15
	vminnm.f32	d31, d31, d31
	vminnm.f32	q0, q0, q0
	vminnm.f32	q8, q8, q8
	vminnm.f32	q7, q7, q7
	vminnm.f32	q15, q15, q15

	.thumb
	vmaxnm.f32	d0, d0, d0
	vmaxnm.f32	d16, d16, d16
	vmaxnm.f32	d15, d15, d15
	vmaxnm.f32	d31, d31, d31
	vmaxnm.f32	q0, q0, q0
	vmaxnm.f32	q8, q8, q8
	vmaxnm.f32	q7, q7, q7
	vmaxnm.f32	q15, q15, q15
	vminnm.f32	d0, d0, d0
	vminnm.f32	d16, d16, d16
	vminnm.f32	d15, d15, d15
	vminnm.f32	d31, d31, d31
	vminnm.f32	q0, q0, q0
	vminnm.f32	q8, q8, q8
	vminnm.f32	q7, q7, q7
	vminnm.f32	q15, q15, q15

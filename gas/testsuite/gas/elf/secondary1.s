	.text

        .size   secondary_function1,1
	.secondary secondary_function1
	.weak	secondary_function1
        .type   secondary_function1,%function
secondary_function1:
	.byte	0x0
        .size   secondary_function1,1
	
        .size   secondary_function2,1
	.weak	secondary_function2
	.secondary secondary_function2
        .type   secondary_function2,%function
secondary_function2:
	.byte	0x0
        .size   secondary_function2,1
	

        .data
        .type   secondary_object1,%object
	.weak	secondary_object1
	.secondary secondary_object1
secondary_object1:
	.byte	0x0
        .size   secondary_object1,1

        .type   secondary_object2,%object
	.weak	secondary_object2
	.secondary secondary_object2
secondary_object2:
	.byte	0x0
        .size   secondary_object2,1

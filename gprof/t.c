void
foo(int x) {
	if (x&3)
		foo (x-1);
}

main() {
	int i;

	for (i=0; i< 1024; i++)
		foo(i);
}

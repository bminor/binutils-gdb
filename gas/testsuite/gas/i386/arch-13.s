# Test -march=
	.text
#SMAP feature
clac
stac
#ADCX ADOX 
adcx    %edx, %ecx
adox    %edx, %ecx
#RDSEED
rdseed    %eax
#CLZERO
clzero
#XSAVEC
xsavec  (%ecx)
#XSAVES
xsaves  (%ecx)
#CLFLUSHOPT
clflushopt      (%ecx)


BEGIN	{
	  FS="\"";
	  print "/* ==> Do not modify this file!!  It is created automatically"
	  print "   by make-c-prog.awk; modify make-c-prog.awk instead.  <== */"
	  print ""
	  print "#include <stdio.h>"
	}

	{
	  if (curfun != FUNCTION)
	    {
	      if (curfun)
		print "}"
	      curfun = FUNCTION
	      print ""
	      print "void";
	      printf "%s (file)\n", FUNCTION
	      print "     FILE *file;";
	      print "{";
	    }
	  printf "  fputs (\"";
	  for (i = 1; i < NF; i++)
	    printf "%s\\\"", $i;
	  printf "%s\\n\", file);\n", $NF;
	}

END	{
	  print "}"
	}

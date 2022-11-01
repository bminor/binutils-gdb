PARSE_AND_LIST_OPTIONS_X86_64_PLT='
  fprintf (file, _("\
  -z mark-plt                 Mark PLT with dynamic tags\n\
  -z nomark-plt               Do not mark PLT with dynamic tags (default)\n"));
'
PARSE_AND_LIST_ARGS_CASE_Z_X86_64_PLT='
      else if (strcmp (optarg, "mark-plt") == 0)
	params.mark_plt = 1;
      else if (strcmp (optarg, "nomark-plt") == 0)
	params.mark_plt = 0;
'

PARSE_AND_LIST_OPTIONS="$PARSE_AND_LIST_OPTIONS $PARSE_AND_LIST_OPTIONS_X86_64_PLT"
PARSE_AND_LIST_ARGS_CASE_Z="$PARSE_AND_LIST_ARGS_CASE_Z $PARSE_AND_LIST_ARGS_CASE_Z_X86_64_PLT"

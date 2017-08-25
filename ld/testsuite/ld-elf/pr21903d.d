#source: pr21903.s
#ld: --no-define-common -pie
#target: *-*-linux* *-*-gnu*
#error: --no-define-common may not be used without -shared

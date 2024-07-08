.*: Assembler messages:
.*: Error: operand mismatch -- `tblq z0.s,{z0.b},z0.b'
.*: Info:    did you mean this\?
.*: Info:    	tblq z0.b, {z0.b}, z0.b
.*: Info:    other valid variant\(s\):
.*: Info:    	tblq z0.h, {z0.h}, z0.h
.*: Info:    	tblq z0.s, {z0.s}, z0.s
.*: Info:    	tblq z0.d, {z0.d}, z0.d
.*: Error: expected a single-register list at operand 2 -- `tblq z0.s,{z0.s,z1.s},z0.s'
.*: Error: expected a single-register list at operand 2 -- `tblq z0.s,{z0.s-z1.s},z0.s'
.*: Error: operand mismatch -- `tblq z0.s,{z31.s},z0.b'
.*: Info:    did you mean this\?
.*: Info:    	tblq z0.s, {z31.s}, z0.s
.*: Info:    other valid variant\(s\):
.*: Info:    	tblq z0.b, {z31.b}, z0.b
.*: Info:    	tblq z0.h, {z31.h}, z0.h
.*: Info:    	tblq z0.d, {z31.d}, z0.d
.*: Error: expected an SVE vector register at operand 3 -- `tblq z0.b,{z0.b},{z31.b}'

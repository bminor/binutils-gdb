.*: Assembler messages:
.*:4: Error: `movbe' is not supported on `x86_64.nomovbe'
.*:5: Error: `movbe' is not supported on `x86_64.nomovbe'
.*:8: Error: `invept' is not supported on `x86_64.noept'
.*:9: Error: `invept' is not supported on `x86_64.noept'
.*:12: Error: `kmovq' is not supported on `x86_64.noavx512bw'
.*:13: Error: `kmovq' is not supported on `x86_64.noavx512bw'
.*:16: Error: `kmovb' is not supported on `x86_64.noavx512dq'
.*:17: Error: `kmovb' is not supported on `x86_64.noavx512dq'
.*:20: Error: `kmovw' is not supported on `x86_64.noavx512f'
.*:21: Error: `kmovw' is not supported on `x86_64.noavx512f'
.*:24: Error: `andn' is not supported on `x86_64.nobmi'
.*:25: Error: `andn' is not supported on `x86_64.nobmi'
.*:28: Error: `bzhi' is not supported on `x86_64.nobmi2'
.*:29: Error: `bzhi' is not supported on `x86_64.nobmi2'
.*:32: Error: .*`%?xmm20'.*`aesenc128kl'.*
.*:33: Error: .*`%?xmm20'.*`sha1msg1'.*
.*:34: Error: .*`%?xmm20'.*`sha256msg2'.*
.*:37: Error: .*`andn'.*
.*:38: Error: .*`bzhi'.*
.*:39: Error: .*`kmovw'.*
.*:40: Error: .*`kmovq'.*
.*:41: Error: .*`kmovb'.*
.*:42: Error: .*`ldtilecfg'.*
.*:43: Error: .*`cmpexadd'.*
GAS LISTING .*
#...
[ 	]*1[ 	]+\# Check illegal 64bit APX EVEX promoted instructions
[ 	]*2[ 	]+\.text
#pass

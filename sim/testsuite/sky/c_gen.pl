#!/usr/local/bin/perl
#
#***********************************************************
#   
#    A tool to read quad-data input and generate a
#    c test-program to help testing PKE/GIF, etc.
#
#    To Invoke:
#       c_gen  <input_data_file> <output_c_file>
#
#    Expected input format:
#       <first column> <second_column> <third column> <forth column>
#       (indicator )    ( quad_word  )  ( source_addr)   (flag)
#       -------------  ---------------  -------------- -------------
#         n (for data)   0xH_H_H_H         0xH           4-CHARs   
#         ? (for test)   0xH  (addr)       0xH  (value)  0xH (mask)
#         ! (reg wrt 32) 0xH  (addr)       0xH  (data) 
#         ~ (reg wrt 64) 0xH  (addr)       0xHigh_Low (data)
#         % (reg read 64) 0xH (addr)       0xHigh_Low (data) 
#         # comment line
#       Note: n can be 0 (for VU1), 1 (for VU2), or 2 (for GIF).
#             H, High, or Low is hex data in the format of FFFFFFFF
#         
#
#    Result output:
#       A c file, either with the name specified, or default.c
#        
#***********************************************************/
 


######################
# Main script:
######################

$numargs = @ARGV;
if ( $numargs == 2 ) {
   $infile_name = $ARGV[0];
   $outfile_name = $ARGV[1];
}
elsif ( $numargs == 1)
{
   $infile_name = $ARGV[0];
   $outfile_name = "default.c"
}
else 
{
   die ("Usage: c_gen <input_data_file_name> <output_c_file_name>\n");
}

# Header containing SCEI system addresses
$date=`date`;
chop($date);

die ("Cannot open input file $infile_name.\n") 
    unless (open (INFILE, $infile_name));
die ("Cannot open output file $outfile_name.\n") 
    unless (open (OUTFILE, ">".$outfile_name));

print ("The input data file is: $infile_name \n");
print ("The output c file is: $outfile_name \n");

&print_header_part_of_c_code;

$current_line_number = 0;

#Now process the input and print the body_part_of_c_code:
while( $inputline = <INFILE> )
{ 
  chop($inputline);           # get rid of the new line char;
  $current_line_number ++;

  print OUTFILE ("/* #line \"$infile_name\" $current_line_number */\n");
  if ($inputline =~ /^\#/ )          # A line starts with "#" is a comment
  {  
      &process_comment;
  } 
  elsif ( $inputline =~ /^[012]/ )   # This is a data line
  {
      &process_data;
  }
  elsif ( $inputline =~ /^\!/ )      # This is a 32-bit register write
  {
      &process_data_reg32;
  }
  elsif ( $inputline =~ /^\~/ )      # This is a 64-bit register write
  {
      &process_data_reg64;
  }
  elsif ( $inputline =~ /^\?/ )      # A line starts with "?" is a 32bit read/verification request
  {
      &perform_test32;
  }
  elsif ( $inputline =~ /^\%/ )      # A line starts with "%" is a 64bit read/verification request
  {
      &perform_test64;
  }
  else   # ignore this input
  {
     print OUTFILE ("\n");
  }
}

&print_foot_part_of_c_code;

close(INFILE);
close(OUTFILE);

print ("Done!\n");
exit(0);




###################
# Subroutines:
###################

sub process_comment {
   $inputline =~ s/#//;
   print OUTFILE ("/*".$inputline."*/\n");
}

sub process_data {
  print OUTFILE ("/*****************************************************************/\n");
  print OUTFILE ("/* Assign a quadword:                                            */\n");

  @columns = split ( /[\s]+/, $inputline );
  $data_count = @columns;

  #column[0] tells to which unit (VU0, VU1, or GIF) these data should go to.
  $src_addr_name = "SRC_ADDR_CONST_".$columns[0];
  $data_addr_name = "DATA_ADDR_CONST_".$columns[0];
  $flag_addr_name = "FLAG_ADDR_CONST_".$columns[0];

  #column[1] is the qual_word in format of 0xH_H_H_H:
  @quadword = split ("_", $columns[1]);
  $quadword[0] =~ s/0x//i;
  
  print OUTFILE ("\n{\n");
    
  print OUTFILE ("  volatile unsigned* flag_ptr = \(unsigned *\)$flag_addr_name;\n");
  print OUTFILE ("  volatile unsigned* src_ptr = \(unsigned *\)$src_addr_name;\n");
  print OUTFILE ("  volatile unsigned* data_ptr = \(unsigned *\)$data_addr_name;\n");

  if ( $data_count > 3 )
  {   #column[3] is the DMA_tag flag, if exist
      $flag = $columns[3];
      if ( $flag =~ /d/i ) {
          print OUTFILE ("  *flag_ptr = 1;\n");
      }
      else {
          print OUTFILE ("  *flag_ptr = 0;\n");
      }
  }

  if ( $data_count > 2 )
  {
     #column[2] is the src_address, if exist
     $src_addr = $columns[2];
     print OUTFILE ("  *src_ptr = $src_addr; \n");
  }

  #Now write the quadword:
  print OUTFILE ("\n");
  print OUTFILE ("  *data_ptr++ = 0x".$quadword[3].";\n");
  print OUTFILE ("  *data_ptr++ = 0x".$quadword[2].";\n");
  print OUTFILE ("  *data_ptr++ = 0x".$quadword[1].";\n");
  print OUTFILE ("  *data_ptr = 0x".$quadword[0].";\n");
  print OUTFILE ("  num_qw_written ++;\n");
  print OUTFILE ("\n");
  
  print OUTFILE ("  *flag_ptr = 0;\n") unless ($data_count < 4);
  print OUTFILE ("  *src_ptr = 0;\n") unless ( $data_count < 3);
  print OUTFILE ("}\n");
}


sub process_data_reg32 {
  print OUTFILE ("\n");
  print OUTFILE ("/******************************************************************/\n");
  print OUTFILE ("/*Writing the specified data into the specified address:          */\n\n");
  
  @columns = split ( /[\s]+/, $inputline );
 
  #column[1] is the address, column[2] is the value, both in the format of 0xH;
  
  print OUTFILE ("\n{\n");
  print OUTFILE ("  volatile unsigned* addr_ptr = \(unsigned *\)".$columns[1].";\n");
  print OUTFILE ("  *addr_ptr = ".$columns[2].";\n");
  print OUTFILE ("  num_w_written ++;\n");
  print OUTFILE ("}\n");
  
}

sub process_data_reg64 {
  print OUTFILE ("\n");
  print OUTFILE ("/******************************************************************/\n");
  print OUTFILE ("/*Writing the specified 64-bit data into the specified address:   */\n\n");
  
  @columns = split ( /[\s]+/, $inputline );
 
  #column[1] is the address, in the format of 0xH;
  #column[2] is the value, in the format of 0xH_H;
  @llword = split ("_", $columns[2]);
  
  print OUTFILE ("\n{\n");
  print OUTFILE ("  volatile long long int* reg64_ptr = \(long long int *\)".$columns[1].";\n");
  print OUTFILE ("  *reg64_ptr = \(long long\)".$llword[0]." \<\< 32 \| \(long long\)0x".$llword[1].";\n");
  print OUTFILE ("  num_w_written ++;\n");
  print OUTFILE ("}\n");
  
}

sub perform_test32 {
  print OUTFILE ("\n");
  print OUTFILE ("/******************************************************************/\n");
  print OUTFILE ("/*Verify the data in the specified address with the input value:  */\n\n");
  
  @columns = split ( /[\s]+/, $inputline );
 
  #column[1] is the address;
  #column[2] is the value, in the format of oxH;
  #column[3] is the mask, in the format of oxH;

  print OUTFILE ("\n{\n");
  print OUTFILE ("  volatile unsigned* test_ptr = \(unsigned *\)".$columns[1].";\n");
  print OUTFILE ("  unsigned test_data = *test_ptr;\n");
  print OUTFILE ("  if \( \( test_data & $columns[3] \) == $columns[2] \) {\n");
  print OUTFILE ("     num_passed ++;\n");
  print OUTFILE ("  } else {\n");
  print OUTFILE ("     printf \(\"Data Verification (line $current_line_number) failed!\\n\"\); \n" );
  print OUTFILE ("     printf \(\"Expecting \%08x mask \%08x in address \%08x but got \%08x !\\n\", $columns[2], $columns[3], $columns[1], test_data\); \n");
  print OUTFILE ("     num_failed++;\n");
  print OUTFILE ("  }\n}\n");

}

sub perform_test64 {
  print OUTFILE ("\n");
  print OUTFILE ("/******************************************************************/\n");
  print OUTFILE ("/*Verify the data in the specified address with the input value:  */\n\n");
  
  @columns = split ( /[\s]+/, $inputline );
 
  #column[1] is the address;
  #column[2] is the value, in the format of 0xH_H;
  @llword = split ("_", $columns[2]);

  print OUTFILE ("\n{\n");
  print OUTFILE ("  volatile long long int* test64_ptr = \(long long int *\)".$columns[1].";\n");
  print OUTFILE ("  long long int test64_data = \(long long\)".$llword[0]." \<\< 32 \| \(long long\)0x".$llword[1].";\n");
  print OUTFILE ("  if \( \( test64_data \) == *test64_ptr \) {\n");
  print OUTFILE ("     num_passed ++;\n");
  print OUTFILE ("  } else {\n");
  print OUTFILE ("     printf \(\"Data Verification (line $current_line_number) failed!\\n\"\); \n" );
  print OUTFILE ("     printf \(\"Expecting \%20s in address \%08x but got \%16x !\\n\", \"$columns[2]\", $columns[1], *test64_ptr\); \n");
  print OUTFILE ("     num_failed++;\n");
  print OUTFILE ("  }\n}\n");

}




sub print_header_part_of_c_code {   

print OUTFILE ("\n/*");
print OUTFILE ("\n * This file is automatically generated.");
$version='$Revision$ $Date$';
print OUTFILE ("\n * c_gen.pl $version");
print OUTFILE ("\n * Input file: $infile_name");
print OUTFILE ("\n * Date: $date");
print OUTFILE ("\n */");
print OUTFILE ("\n");
print OUTFILE ("\n#include <stdio.h>\n");
print OUTFILE ("\n");
print OUTFILE ("
/* Memory mapping constants:  */

/* VIF0 */
#define SRC_ADDR_CONST_0  0x10008010
#define DATA_ADDR_CONST_0 0x10004000
#define FLAG_ADDR_CONST_0 0x10008060

/* VIF1 */
#define SRC_ADDR_CONST_1  0x10009010
#define DATA_ADDR_CONST_1 0x10005000
#define FLAG_ADDR_CONST_1 0x10009060

/* GIF */
#define SRC_ADDR_CONST_2  0x1000a010
#define DATA_ADDR_CONST_2 0x10006000
#define FLAG_ADDR_CONST_2 0x1000a060
");
print OUTFILE ("\n\n");

print OUTFILE ("int main()\n");
print OUTFILE ("{\n");
print OUTFILE (" unsigned num_qw_written = 0;\n");
print OUTFILE (" unsigned num_w_written = 0;\n");
print OUTFILE (" unsigned num_passed = 0;\n");
print OUTFILE (" unsigned num_failed = 0;\n");
print OUTFILE (" printf \(\"Start of execution...\\n\"\); \n" );
print OUTFILE ("\n\n");
}

sub print_foot_part_of_c_code {

print OUTFILE ("\n");
print OUTFILE (" printf \(\"End of execution. %d FIFO quadwords, %d pokes, %d checks ok, %d failed.\\n\", num_qw_written, num_w_written, num_passed, num_failed\); \n\n" );
print OUTFILE ("exit (num_failed);\n");
print OUTFILE ("}\n");
}

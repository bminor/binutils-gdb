#define BFD_VERSION_DATE 20151124
#define BFD_VERSION @bfd_version@
/* Various parts of GCC parse Binutils version string in different fragile ways,
   and we need to be very careful to not break them.  So far it seems that
   appending Linaro-specific version at the end is fine so long as it doesn't
   contain any dots ".".  */
#define BFD_VERSION_STRING  @bfd_version_package@ @bfd_version_string@ " Linaro 2016_02"
#define REPORT_BUGS_TO @report_bugs_to@

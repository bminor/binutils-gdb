#as: --gsframe
#source: sframe-foo.s
#source: sframe-bar.s
#readelf: --sframe
#ld: -shared --no-rosegment -z separate-code --no-sframe-info
#name: Command line option --no-sframe-info

Section '.sframe' has no debugging data.

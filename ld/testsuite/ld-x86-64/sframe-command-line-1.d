#as: --gsframe
#source: sframe-foo.s
#source: sframe-bar.s
#readelf: --sframe
#ld: -shared --no-rosegment -z separate-code --discard-sframe-sections
#name: Command line option --discard-sframe-sections

Section '.sframe' has no debugging data.

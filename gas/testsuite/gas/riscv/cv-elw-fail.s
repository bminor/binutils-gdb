target:
    # Immediate Boundary Tests
    cv.elw x5,-2049(x6)
    cv.elw x5,2048(x6)

    # Register Boundary Tests
    cv.elw x-1,1024(x-1)
    cv.elw x32,1024(x32)

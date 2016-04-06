case ${target} in
    arceb-*)
        ARC_ENDIAN="big"
        ;;
    arc-*)
        ARC_ENDIAN="little"
        ;;
    *)
        exit 1
esac

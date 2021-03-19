# Vector AMO Operations

	vamoaddei8.v v0, (a1), v4, v0		# OK
	vamoaddei8.v v4, (a1), v4, v0		# vd must match vs3
	vamoaddei8.v v0, (a1), v4, v0, v0.t	# vd overlap vm
	vamoaddei8.v x0, (a1), v4, v0		# OK
	vamoaddei8.v x0, (a1), v4, v0, v0.t	# vs3 overlap vm

	vamoswapei8.v v0, (a1), v4, v0
	vamoswapei8.v v4, (a1), v4, v0
	vamoswapei8.v v0, (a1), v4, v0, v0.t
	vamoswapei8.v x0, (a1), v4, v0
	vamoswapei8.v x0, (a1), v4, v0, v0.t

	vamoxorei8.v v0, (a1), v4, v0
	vamoxorei8.v v4, (a1), v4, v0
	vamoxorei8.v v0, (a1), v4, v0, v0.t
	vamoxorei8.v x0, (a1), v4, v0
	vamoxorei8.v x0, (a1), v4, v0, v0.t

	vamoandei8.v v0, (a1), v4, v0
	vamoandei8.v v4, (a1), v4, v0
	vamoandei8.v v0, (a1), v4, v0, v0.t
	vamoandei8.v x0, (a1), v4, v0
	vamoandei8.v x0, (a1), v4, v0, v0.t

	vamoorei8.v v0, (a1), v4, v0
	vamoorei8.v v4, (a1), v4, v0
	vamoorei8.v v0, (a1), v4, v0, v0.t
	vamoorei8.v x0, (a1), v4, v0
	vamoorei8.v x0, (a1), v4, v0, v0.t

	vamominei8.v v0, (a1), v4, v0
	vamominei8.v v4, (a1), v4, v0
	vamominei8.v v0, (a1), v4, v0, v0.t
	vamominei8.v x0, (a1), v4, v0
	vamominei8.v x0, (a1), v4, v0, v0.t

	vamomaxei8.v v0, (a1), v4, v0
	vamomaxei8.v v4, (a1), v4, v0
	vamomaxei8.v v0, (a1), v4, v0, v0.t
	vamomaxei8.v x0, (a1), v4, v0
	vamomaxei8.v x0, (a1), v4, v0, v0.t

	vamominuei8.v v0, (a1), v4, v0
	vamominuei8.v v4, (a1), v4, v0
	vamominuei8.v v0, (a1), v4, v0, v0.t
	vamominuei8.v x0, (a1), v4, v0
	vamominuei8.v x0, (a1), v4, v0, v0.t

	vamomaxuei8.v v0, (a1), v4, v0
	vamomaxuei8.v v4, (a1), v4, v0
	vamomaxuei8.v v0, (a1), v4, v0, v0.t
	vamomaxuei8.v x0, (a1), v4, v0
	vamomaxuei8.v x0, (a1), v4, v0, v0.t

	vamoaddei16.v v0, (a1), v4, v0
	vamoaddei16.v v4, (a1), v4, v0
	vamoaddei16.v v0, (a1), v4, v0, v0.t
	vamoaddei16.v x0, (a1), v4, v0
	vamoaddei16.v x0, (a1), v4, v0, v0.t

	vamoswapei16.v v0, (a1), v4, v0
	vamoswapei16.v v0, (a1), v4, v0, v0.t
	vamoswapei16.v v4, (a1), v4, v0
	vamoswapei16.v x0, (a1), v4, v0
	vamoswapei16.v x0, (a1), v4, v0, v0.t

	vamoxorei16.v v0, (a1), v4, v0
	vamoxorei16.v v0, (a1), v4, v0, v0.t
	vamoxorei16.v v4, (a1), v4, v0
	vamoxorei16.v x0, (a1), v4, v0
	vamoxorei16.v x0, (a1), v4, v0, v0.t

	vamoandei16.v v0, (a1), v4, v0
	vamoandei16.v v0, (a1), v4, v0, v0.t
	vamoandei16.v v4, (a1), v4, v0
	vamoandei16.v x0, (a1), v4, v0
	vamoandei16.v x0, (a1), v4, v0, v0.t

	vamoorei16.v v0, (a1), v4, v0
	vamoorei16.v v0, (a1), v4, v0, v0.t
	vamoorei16.v v4, (a1), v4, v0
	vamoorei16.v x0, (a1), v4, v0
	vamoorei16.v x0, (a1), v4, v0, v0.t

	vamominei16.v v0, (a1), v4, v0
	vamominei16.v v0, (a1), v4, v0, v0.t
	vamominei16.v v4, (a1), v4, v0
	vamominei16.v x0, (a1), v4, v0
	vamominei16.v x0, (a1), v4, v0, v0.t

	vamomaxei16.v v0, (a1), v4, v0
	vamomaxei16.v v0, (a1), v4, v0, v0.t
	vamomaxei16.v v4, (a1), v4, v0
	vamomaxei16.v x0, (a1), v4, v0
	vamomaxei16.v x0, (a1), v4, v0, v0.t

	vamominuei16.v v0, (a1), v4, v0
	vamominuei16.v v0, (a1), v4, v0, v0.t
	vamominuei16.v v4, (a1), v4, v0
	vamominuei16.v x0, (a1), v4, v0
	vamominuei16.v x0, (a1), v4, v0, v0.t

	vamomaxuei16.v v0, (a1), v4, v0
	vamomaxuei16.v v0, (a1), v4, v0, v0.t
	vamomaxuei16.v v4, (a1), v4, v0
	vamomaxuei16.v x0, (a1), v4, v0
	vamomaxuei16.v x0, (a1), v4, v0, v0.t

	vamoaddei32.v v0, (a1), v4, v0
	vamoaddei32.v v0, (a1), v4, v0, v0.t
	vamoaddei32.v v4, (a1), v4, v0
	vamoaddei32.v x0, (a1), v4, v0
	vamoaddei32.v x0, (a1), v4, v0, v0.t

	vamoswapei32.v v0, (a1), v4, v0
	vamoswapei32.v v4, (a1), v4, v0
	vamoswapei32.v v0, (a1), v4, v0, v0.t
	vamoswapei32.v x0, (a1), v4, v0
	vamoswapei32.v x0, (a1), v4, v0, v0.t

	vamoxorei32.v v0, (a1), v4, v0
	vamoxorei32.v v4, (a1), v4, v0
	vamoxorei32.v v0, (a1), v4, v0, v0.t
	vamoxorei32.v x0, (a1), v4, v0
	vamoxorei32.v x0, (a1), v4, v0, v0.t

	vamoandei32.v v0, (a1), v4, v0
	vamoandei32.v v4, (a1), v4, v0
	vamoandei32.v v0, (a1), v4, v0, v0.t
	vamoandei32.v x0, (a1), v4, v0
	vamoandei32.v x0, (a1), v4, v0, v0.t

	vamoorei32.v v0, (a1), v4, v0
	vamoorei32.v v4, (a1), v4, v0
	vamoorei32.v v0, (a1), v4, v0, v0.t
	vamoorei32.v x0, (a1), v4, v0
	vamoorei32.v x0, (a1), v4, v0, v0.t

	vamominei32.v v0, (a1), v4, v0
	vamominei32.v v4, (a1), v4, v0
	vamominei32.v v0, (a1), v4, v0, v0.t
	vamominei32.v x0, (a1), v4, v0
	vamominei32.v x0, (a1), v4, v0, v0.t

	vamomaxei32.v v0, (a1), v4, v0
	vamomaxei32.v v4, (a1), v4, v0
	vamomaxei32.v v0, (a1), v4, v0, v0.t
	vamomaxei32.v x0, (a1), v4, v0
	vamomaxei32.v x0, (a1), v4, v0, v0.t

	vamominuei32.v v0, (a1), v4, v0
	vamominuei32.v v4, (a1), v4, v0
	vamominuei32.v v0, (a1), v4, v0, v0.t
	vamominuei32.v x0, (a1), v4, v0
	vamominuei32.v x0, (a1), v4, v0, v0.t

	vamomaxuei32.v v0, (a1), v4, v0
	vamomaxuei32.v v4, (a1), v4, v0
	vamomaxuei32.v v0, (a1), v4, v0, v0.t
	vamomaxuei32.v x0, (a1), v4, v0
	vamomaxuei32.v x0, (a1), v4, v0, v0.t

	vamoaddei64.v v0, (a1), v4, v0
	vamoaddei64.v v4, (a1), v4, v0
	vamoaddei64.v v0, (a1), v4, v0, v0.t
	vamoaddei64.v x0, (a1), v4, v0
	vamoaddei64.v x0, (a1), v4, v0, v0.t

	vamoswapei64.v v0, (a1), v4, v0
	vamoswapei64.v v4, (a1), v4, v0
	vamoswapei64.v v0, (a1), v4, v0, v0.t
	vamoswapei64.v x0, (a1), v4, v0
	vamoswapei64.v x0, (a1), v4, v0, v0.t

	vamoxorei64.v v0, (a1), v4, v0
	vamoxorei64.v v4, (a1), v4, v0
	vamoxorei64.v v0, (a1), v4, v0, v0.t
	vamoxorei64.v x0, (a1), v4, v0
	vamoxorei64.v x0, (a1), v4, v0, v0.t

	vamoandei64.v v0, (a1), v4, v0
	vamoandei64.v v4, (a1), v4, v0
	vamoandei64.v v0, (a1), v4, v0, v0.t
	vamoandei64.v x0, (a1), v4, v0
	vamoandei64.v x0, (a1), v4, v0, v0.t

	vamoorei64.v v0, (a1), v4, v0
	vamoorei64.v v4, (a1), v4, v0
	vamoorei64.v v0, (a1), v4, v0, v0.t
	vamoorei64.v x0, (a1), v4, v0
	vamoorei64.v x0, (a1), v4, v0, v0.t

	vamominei64.v v0, (a1), v4, v0
	vamominei64.v v4, (a1), v4, v0
	vamominei64.v v0, (a1), v4, v0, v0.t
	vamominei64.v x0, (a1), v4, v0
	vamominei64.v x0, (a1), v4, v0, v0.t

	vamomaxei64.v v0, (a1), v4, v0
	vamomaxei64.v v4, (a1), v4, v0
	vamomaxei64.v v0, (a1), v4, v0, v0.t
	vamomaxei64.v x0, (a1), v4, v0
	vamomaxei64.v x0, (a1), v4, v0, v0.t

	vamominuei64.v v0, (a1), v4, v0
	vamominuei64.v v4, (a1), v4, v0
	vamominuei64.v v0, (a1), v4, v0, v0.t
	vamominuei64.v x0, (a1), v4, v0
	vamominuei64.v x0, (a1), v4, v0, v0.t

	vamomaxuei64.v v0, (a1), v4, v0
	vamomaxuei64.v v4, (a1), v4, v0
	vamomaxuei64.v v0, (a1), v4, v0, v0.t
	vamomaxuei64.v x0, (a1), v4, v0
	vamomaxuei64.v x0, (a1), v4, v0, v0.t

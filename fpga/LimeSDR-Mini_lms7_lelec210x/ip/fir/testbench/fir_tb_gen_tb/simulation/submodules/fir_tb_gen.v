// fir_tb_gen.v

// Generated using ACDS version 18.1 625

`timescale 1 ps / 1 ps
module fir_tb_gen (
		input  wire        clk_clk,                                         //                                       clk.clk
		input  wire [23:0] fir_compiler_ii_0_avalon_streaming_sink_data,    //   fir_compiler_ii_0_avalon_streaming_sink.data
		input  wire        fir_compiler_ii_0_avalon_streaming_sink_valid,   //                                          .valid
		input  wire [1:0]  fir_compiler_ii_0_avalon_streaming_sink_error,   //                                          .error
		output wire [23:0] fir_compiler_ii_0_avalon_streaming_source_data,  // fir_compiler_ii_0_avalon_streaming_source.data
		output wire        fir_compiler_ii_0_avalon_streaming_source_valid, //                                          .valid
		output wire [1:0]  fir_compiler_ii_0_avalon_streaming_source_error, //                                          .error
		input  wire        reset_reset_n                                    //                                     reset.reset_n
	);

	wire    rst_controller_reset_out_reset; // rst_controller:reset_out -> fir_compiler_ii_0:reset_n

	fir_tb_gen_fir_compiler_ii_0 fir_compiler_ii_0 (
		.clk              (clk_clk),                                         //                     clk.clk
		.reset_n          (~rst_controller_reset_out_reset),                 //                     rst.reset_n
		.ast_sink_data    (fir_compiler_ii_0_avalon_streaming_sink_data),    //   avalon_streaming_sink.data
		.ast_sink_valid   (fir_compiler_ii_0_avalon_streaming_sink_valid),   //                        .valid
		.ast_sink_error   (fir_compiler_ii_0_avalon_streaming_sink_error),   //                        .error
		.ast_source_data  (fir_compiler_ii_0_avalon_streaming_source_data),  // avalon_streaming_source.data
		.ast_source_valid (fir_compiler_ii_0_avalon_streaming_source_valid), //                        .valid
		.ast_source_error (fir_compiler_ii_0_avalon_streaming_source_error)  //                        .error
	);

	altera_reset_controller #(
		.NUM_RESET_INPUTS          (1),
		.OUTPUT_RESET_SYNC_EDGES   ("deassert"),
		.SYNC_DEPTH                (2),
		.RESET_REQUEST_PRESENT     (0),
		.RESET_REQ_WAIT_TIME       (1),
		.MIN_RST_ASSERTION_TIME    (3),
		.RESET_REQ_EARLY_DSRT_TIME (1),
		.USE_RESET_REQUEST_IN0     (0),
		.USE_RESET_REQUEST_IN1     (0),
		.USE_RESET_REQUEST_IN2     (0),
		.USE_RESET_REQUEST_IN3     (0),
		.USE_RESET_REQUEST_IN4     (0),
		.USE_RESET_REQUEST_IN5     (0),
		.USE_RESET_REQUEST_IN6     (0),
		.USE_RESET_REQUEST_IN7     (0),
		.USE_RESET_REQUEST_IN8     (0),
		.USE_RESET_REQUEST_IN9     (0),
		.USE_RESET_REQUEST_IN10    (0),
		.USE_RESET_REQUEST_IN11    (0),
		.USE_RESET_REQUEST_IN12    (0),
		.USE_RESET_REQUEST_IN13    (0),
		.USE_RESET_REQUEST_IN14    (0),
		.USE_RESET_REQUEST_IN15    (0),
		.ADAPT_RESET_REQUEST       (0)
	) rst_controller (
		.reset_in0      (~reset_reset_n),                 // reset_in0.reset
		.clk            (clk_clk),                        //       clk.clk
		.reset_out      (rst_controller_reset_out_reset), // reset_out.reset
		.reset_req      (),                               // (terminated)
		.reset_req_in0  (1'b0),                           // (terminated)
		.reset_in1      (1'b0),                           // (terminated)
		.reset_req_in1  (1'b0),                           // (terminated)
		.reset_in2      (1'b0),                           // (terminated)
		.reset_req_in2  (1'b0),                           // (terminated)
		.reset_in3      (1'b0),                           // (terminated)
		.reset_req_in3  (1'b0),                           // (terminated)
		.reset_in4      (1'b0),                           // (terminated)
		.reset_req_in4  (1'b0),                           // (terminated)
		.reset_in5      (1'b0),                           // (terminated)
		.reset_req_in5  (1'b0),                           // (terminated)
		.reset_in6      (1'b0),                           // (terminated)
		.reset_req_in6  (1'b0),                           // (terminated)
		.reset_in7      (1'b0),                           // (terminated)
		.reset_req_in7  (1'b0),                           // (terminated)
		.reset_in8      (1'b0),                           // (terminated)
		.reset_req_in8  (1'b0),                           // (terminated)
		.reset_in9      (1'b0),                           // (terminated)
		.reset_req_in9  (1'b0),                           // (terminated)
		.reset_in10     (1'b0),                           // (terminated)
		.reset_req_in10 (1'b0),                           // (terminated)
		.reset_in11     (1'b0),                           // (terminated)
		.reset_req_in11 (1'b0),                           // (terminated)
		.reset_in12     (1'b0),                           // (terminated)
		.reset_req_in12 (1'b0),                           // (terminated)
		.reset_in13     (1'b0),                           // (terminated)
		.reset_req_in13 (1'b0),                           // (terminated)
		.reset_in14     (1'b0),                           // (terminated)
		.reset_req_in14 (1'b0),                           // (terminated)
		.reset_in15     (1'b0),                           // (terminated)
		.reset_req_in15 (1'b0)                            // (terminated)
	);

endmodule

module led(
				input ext_clk_25m,
				input ext_rst_n,
				output reg[7:0]led
				);
				
	reg[19:0]cnt;
	
	always@(posedge ext_clk_25m or negedge ext_rst_n)
		if(!ext_rst_n) cnt <= 20'd0;
		else cnt <= cnt + 1'b1;
		
		
	always@(posedge ext_clk_25m or negedge ext_rst_n)
		if(!ext_rst_n) led <= 8'd1111_1110;
		else if(cnt == 20'hfffff) led <= {led[6:0],led[7]};
		else;
		
endmodule
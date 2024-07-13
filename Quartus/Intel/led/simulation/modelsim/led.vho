-- Copyright (C) 1991-2013 Altera Corporation
-- Your use of Altera Corporation's design tools, logic functions 
-- and other software and tools, and its AMPP partner logic 
-- functions, and any output files from any of the foregoing 
-- (including device programming or simulation files), and any 
-- associated documentation or information are expressly subject 
-- to the terms and conditions of the Altera Program License 
-- Subscription Agreement, Altera MegaCore Function License 
-- Agreement, or other applicable license agreement, including, 
-- without limitation, that your use is for the sole purpose of 
-- programming logic devices manufactured by Altera and sold by 
-- Altera or its authorized distributors.  Please refer to the 
-- applicable agreement for further details.

-- VENDOR "Altera"
-- PROGRAM "Quartus II 64-Bit"
-- VERSION "Version 13.0.1 Build 232 06/12/2013 Service Pack 1 SJ Full Version"

-- DATE "04/15/2024 21:17:10"

-- 
-- Device: Altera EP4CE6E22C8 Package TQFP144
-- 

-- 
-- This VHDL file should be used for ModelSim-Altera (VHDL) only
-- 

LIBRARY ALTERA;
LIBRARY CYCLONEIVE;
LIBRARY IEEE;
USE ALTERA.ALTERA_PRIMITIVES_COMPONENTS.ALL;
USE CYCLONEIVE.CYCLONEIVE_COMPONENTS.ALL;
USE IEEE.STD_LOGIC_1164.ALL;

ENTITY 	led IS
    PORT (
	ext_clk_25m : IN std_logic;
	ext_rst_n : IN std_logic;
	led : OUT std_logic_vector(7 DOWNTO 0)
	);
END led;

-- Design Ports Information
-- led[0]	=>  Location: PIN_112,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[1]	=>  Location: PIN_121,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[2]	=>  Location: PIN_119,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[3]	=>  Location: PIN_120,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[4]	=>  Location: PIN_124,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[5]	=>  Location: PIN_126,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[6]	=>  Location: PIN_115,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- led[7]	=>  Location: PIN_125,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- ext_clk_25m	=>  Location: PIN_23,	 I/O Standard: 2.5 V,	 Current Strength: Default
-- ext_rst_n	=>  Location: PIN_24,	 I/O Standard: 2.5 V,	 Current Strength: Default


ARCHITECTURE structure OF led IS
SIGNAL gnd : std_logic := '0';
SIGNAL vcc : std_logic := '1';
SIGNAL unknown : std_logic := 'X';
SIGNAL devoe : std_logic := '1';
SIGNAL devclrn : std_logic := '1';
SIGNAL devpor : std_logic := '1';
SIGNAL ww_devoe : std_logic;
SIGNAL ww_devclrn : std_logic;
SIGNAL ww_devpor : std_logic;
SIGNAL ww_ext_clk_25m : std_logic;
SIGNAL ww_ext_rst_n : std_logic;
SIGNAL ww_led : std_logic_vector(7 DOWNTO 0);
SIGNAL \ext_rst_n~inputclkctrl_INCLK_bus\ : std_logic_vector(3 DOWNTO 0);
SIGNAL \ext_clk_25m~inputclkctrl_INCLK_bus\ : std_logic_vector(3 DOWNTO 0);
SIGNAL \cnt[5]~27_combout\ : std_logic;
SIGNAL \cnt[13]~43_combout\ : std_logic;
SIGNAL \cnt[15]~47_combout\ : std_logic;
SIGNAL \cnt[16]~49_combout\ : std_logic;
SIGNAL \Equal0~5_combout\ : std_logic;
SIGNAL \cnt[0]~57_combout\ : std_logic;
SIGNAL \led[0]~output_o\ : std_logic;
SIGNAL \led[1]~output_o\ : std_logic;
SIGNAL \led[2]~output_o\ : std_logic;
SIGNAL \led[3]~output_o\ : std_logic;
SIGNAL \led[4]~output_o\ : std_logic;
SIGNAL \led[5]~output_o\ : std_logic;
SIGNAL \led[6]~output_o\ : std_logic;
SIGNAL \led[7]~output_o\ : std_logic;
SIGNAL \ext_clk_25m~input_o\ : std_logic;
SIGNAL \ext_clk_25m~inputclkctrl_outclk\ : std_logic;
SIGNAL \led[1]~1_combout\ : std_logic;
SIGNAL \ext_rst_n~input_o\ : std_logic;
SIGNAL \ext_rst_n~inputclkctrl_outclk\ : std_logic;
SIGNAL \cnt[1]~19_combout\ : std_logic;
SIGNAL \cnt[1]~20\ : std_logic;
SIGNAL \cnt[2]~21_combout\ : std_logic;
SIGNAL \cnt[2]~22\ : std_logic;
SIGNAL \cnt[3]~23_combout\ : std_logic;
SIGNAL \cnt[3]~24\ : std_logic;
SIGNAL \cnt[4]~25_combout\ : std_logic;
SIGNAL \cnt[4]~26\ : std_logic;
SIGNAL \cnt[5]~28\ : std_logic;
SIGNAL \cnt[6]~29_combout\ : std_logic;
SIGNAL \cnt[6]~30\ : std_logic;
SIGNAL \cnt[7]~32\ : std_logic;
SIGNAL \cnt[8]~33_combout\ : std_logic;
SIGNAL \cnt[8]~34\ : std_logic;
SIGNAL \cnt[9]~36\ : std_logic;
SIGNAL \cnt[10]~37_combout\ : std_logic;
SIGNAL \cnt[10]~38\ : std_logic;
SIGNAL \cnt[11]~39_combout\ : std_logic;
SIGNAL \cnt[11]~40\ : std_logic;
SIGNAL \cnt[12]~41_combout\ : std_logic;
SIGNAL \cnt[12]~42\ : std_logic;
SIGNAL \cnt[13]~44\ : std_logic;
SIGNAL \cnt[14]~45_combout\ : std_logic;
SIGNAL \cnt[14]~46\ : std_logic;
SIGNAL \cnt[15]~48\ : std_logic;
SIGNAL \cnt[16]~50\ : std_logic;
SIGNAL \cnt[17]~51_combout\ : std_logic;
SIGNAL \cnt[17]~52\ : std_logic;
SIGNAL \cnt[18]~53_combout\ : std_logic;
SIGNAL \cnt[18]~54\ : std_logic;
SIGNAL \cnt[19]~55_combout\ : std_logic;
SIGNAL \Equal0~0_combout\ : std_logic;
SIGNAL \cnt[9]~35_combout\ : std_logic;
SIGNAL \Equal0~4_combout\ : std_logic;
SIGNAL \cnt[7]~31_combout\ : std_logic;
SIGNAL \Equal0~2_combout\ : std_logic;
SIGNAL \Equal0~1_combout\ : std_logic;
SIGNAL \Equal0~3_combout\ : std_logic;
SIGNAL \Equal0~6_combout\ : std_logic;
SIGNAL \led[1]~reg0_q\ : std_logic;
SIGNAL \led[2]~reg0feeder_combout\ : std_logic;
SIGNAL \led[2]~reg0_q\ : std_logic;
SIGNAL \led[3]~2_combout\ : std_logic;
SIGNAL \led[3]~reg0_q\ : std_logic;
SIGNAL \led[4]~reg0feeder_combout\ : std_logic;
SIGNAL \led[4]~reg0_q\ : std_logic;
SIGNAL \led[5]~reg0feeder_combout\ : std_logic;
SIGNAL \led[5]~reg0_q\ : std_logic;
SIGNAL \led[6]~3_combout\ : std_logic;
SIGNAL \led[6]~reg0_q\ : std_logic;
SIGNAL \led[7]~reg0feeder_combout\ : std_logic;
SIGNAL \led[7]~reg0_q\ : std_logic;
SIGNAL \led[0]~0_combout\ : std_logic;
SIGNAL \led[0]~reg0_q\ : std_logic;
SIGNAL cnt : std_logic_vector(19 DOWNTO 0);
SIGNAL \ALT_INV_led[7]~reg0_q\ : std_logic;
SIGNAL \ALT_INV_led[6]~reg0_q\ : std_logic;
SIGNAL \ALT_INV_led[2]~reg0_q\ : std_logic;
SIGNAL \ALT_INV_led[1]~reg0_q\ : std_logic;

BEGIN

ww_ext_clk_25m <= ext_clk_25m;
ww_ext_rst_n <= ext_rst_n;
led <= ww_led;
ww_devoe <= devoe;
ww_devclrn <= devclrn;
ww_devpor <= devpor;

\ext_rst_n~inputclkctrl_INCLK_bus\ <= (vcc & vcc & vcc & \ext_rst_n~input_o\);

\ext_clk_25m~inputclkctrl_INCLK_bus\ <= (vcc & vcc & vcc & \ext_clk_25m~input_o\);
\ALT_INV_led[7]~reg0_q\ <= NOT \led[7]~reg0_q\;
\ALT_INV_led[6]~reg0_q\ <= NOT \led[6]~reg0_q\;
\ALT_INV_led[2]~reg0_q\ <= NOT \led[2]~reg0_q\;
\ALT_INV_led[1]~reg0_q\ <= NOT \led[1]~reg0_q\;

-- Location: FF_X25_Y22_N13
\cnt[16]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[16]~49_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(16));

-- Location: FF_X25_Y23_N23
\cnt[5]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[5]~27_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(5));

-- Location: FF_X25_Y22_N7
\cnt[13]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[13]~43_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(13));

-- Location: FF_X25_Y22_N11
\cnt[15]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[15]~47_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(15));

-- Location: LCCOMB_X25_Y23_N22
\cnt[5]~27\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[5]~27_combout\ = (cnt(5) & (\cnt[4]~26\ $ (GND))) # (!cnt(5) & (!\cnt[4]~26\ & VCC))
-- \cnt[5]~28\ = CARRY((cnt(5) & !\cnt[4]~26\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(5),
	datad => VCC,
	cin => \cnt[4]~26\,
	combout => \cnt[5]~27_combout\,
	cout => \cnt[5]~28\);

-- Location: LCCOMB_X25_Y22_N6
\cnt[13]~43\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[13]~43_combout\ = (cnt(13) & (\cnt[12]~42\ $ (GND))) # (!cnt(13) & (!\cnt[12]~42\ & VCC))
-- \cnt[13]~44\ = CARRY((cnt(13) & !\cnt[12]~42\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(13),
	datad => VCC,
	cin => \cnt[12]~42\,
	combout => \cnt[13]~43_combout\,
	cout => \cnt[13]~44\);

-- Location: LCCOMB_X25_Y22_N10
\cnt[15]~47\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[15]~47_combout\ = (cnt(15) & (\cnt[14]~46\ $ (GND))) # (!cnt(15) & (!\cnt[14]~46\ & VCC))
-- \cnt[15]~48\ = CARRY((cnt(15) & !\cnt[14]~46\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(15),
	datad => VCC,
	cin => \cnt[14]~46\,
	combout => \cnt[15]~47_combout\,
	cout => \cnt[15]~48\);

-- Location: LCCOMB_X25_Y22_N12
\cnt[16]~49\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[16]~49_combout\ = (cnt(16) & (!\cnt[15]~48\)) # (!cnt(16) & ((\cnt[15]~48\) # (GND)))
-- \cnt[16]~50\ = CARRY((!\cnt[15]~48\) # (!cnt(16)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0101101001011111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(16),
	datad => VCC,
	cin => \cnt[15]~48\,
	combout => \cnt[16]~49_combout\,
	cout => \cnt[16]~50\);

-- Location: FF_X25_Y23_N13
\cnt[0]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[0]~57_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(0));

-- Location: LCCOMB_X25_Y22_N30
\Equal0~5\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~5_combout\ = (cnt(15) & (cnt(14) & (cnt(12) & cnt(13))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(15),
	datab => cnt(14),
	datac => cnt(12),
	datad => cnt(13),
	combout => \Equal0~5_combout\);

-- Location: LCCOMB_X25_Y23_N12
\cnt[0]~57\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[0]~57_combout\ = !cnt(0)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000111100001111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => cnt(0),
	combout => \cnt[0]~57_combout\);

-- Location: IOOBUF_X28_Y24_N2
\led[0]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \led[0]~reg0_q\,
	devoe => ww_devoe,
	o => \led[0]~output_o\);

-- Location: IOOBUF_X23_Y24_N16
\led[1]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \ALT_INV_led[1]~reg0_q\,
	devoe => ww_devoe,
	o => \led[1]~output_o\);

-- Location: IOOBUF_X23_Y24_N2
\led[2]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \ALT_INV_led[2]~reg0_q\,
	devoe => ww_devoe,
	o => \led[2]~output_o\);

-- Location: IOOBUF_X23_Y24_N9
\led[3]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \led[3]~reg0_q\,
	devoe => ww_devoe,
	o => \led[3]~output_o\);

-- Location: IOOBUF_X18_Y24_N16
\led[4]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \led[4]~reg0_q\,
	devoe => ww_devoe,
	o => \led[4]~output_o\);

-- Location: IOOBUF_X16_Y24_N2
\led[5]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \led[5]~reg0_q\,
	devoe => ww_devoe,
	o => \led[5]~output_o\);

-- Location: IOOBUF_X28_Y24_N23
\led[6]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \ALT_INV_led[6]~reg0_q\,
	devoe => ww_devoe,
	o => \led[6]~output_o\);

-- Location: IOOBUF_X18_Y24_N23
\led[7]~output\ : cycloneive_io_obuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	open_drain_output => "false")
-- pragma translate_on
PORT MAP (
	i => \ALT_INV_led[7]~reg0_q\,
	devoe => ww_devoe,
	o => \led[7]~output_o\);

-- Location: IOIBUF_X0_Y11_N8
\ext_clk_25m~input\ : cycloneive_io_ibuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	simulate_z_as => "z")
-- pragma translate_on
PORT MAP (
	i => ww_ext_clk_25m,
	o => \ext_clk_25m~input_o\);

-- Location: CLKCTRL_G2
\ext_clk_25m~inputclkctrl\ : cycloneive_clkctrl
-- pragma translate_off
GENERIC MAP (
	clock_type => "global clock",
	ena_register_mode => "none")
-- pragma translate_on
PORT MAP (
	inclk => \ext_clk_25m~inputclkctrl_INCLK_bus\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	outclk => \ext_clk_25m~inputclkctrl_outclk\);

-- Location: LCCOMB_X24_Y22_N2
\led[1]~1\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[1]~1_combout\ = !\led[0]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[0]~reg0_q\,
	combout => \led[1]~1_combout\);

-- Location: IOIBUF_X0_Y11_N15
\ext_rst_n~input\ : cycloneive_io_ibuf
-- pragma translate_off
GENERIC MAP (
	bus_hold => "false",
	simulate_z_as => "z")
-- pragma translate_on
PORT MAP (
	i => ww_ext_rst_n,
	o => \ext_rst_n~input_o\);

-- Location: CLKCTRL_G4
\ext_rst_n~inputclkctrl\ : cycloneive_clkctrl
-- pragma translate_off
GENERIC MAP (
	clock_type => "global clock",
	ena_register_mode => "none")
-- pragma translate_on
PORT MAP (
	inclk => \ext_rst_n~inputclkctrl_INCLK_bus\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	outclk => \ext_rst_n~inputclkctrl_outclk\);

-- Location: LCCOMB_X25_Y23_N14
\cnt[1]~19\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[1]~19_combout\ = (cnt(0) & (cnt(1) $ (VCC))) # (!cnt(0) & (cnt(1) & VCC))
-- \cnt[1]~20\ = CARRY((cnt(0) & cnt(1)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0110011010001000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(0),
	datab => cnt(1),
	datad => VCC,
	combout => \cnt[1]~19_combout\,
	cout => \cnt[1]~20\);

-- Location: FF_X25_Y23_N15
\cnt[1]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[1]~19_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(1));

-- Location: LCCOMB_X25_Y23_N16
\cnt[2]~21\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[2]~21_combout\ = (cnt(2) & (!\cnt[1]~20\)) # (!cnt(2) & ((\cnt[1]~20\) # (GND)))
-- \cnt[2]~22\ = CARRY((!\cnt[1]~20\) # (!cnt(2)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(2),
	datad => VCC,
	cin => \cnt[1]~20\,
	combout => \cnt[2]~21_combout\,
	cout => \cnt[2]~22\);

-- Location: FF_X25_Y23_N17
\cnt[2]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[2]~21_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(2));

-- Location: LCCOMB_X25_Y23_N18
\cnt[3]~23\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[3]~23_combout\ = (cnt(3) & (\cnt[2]~22\ $ (GND))) # (!cnt(3) & (!\cnt[2]~22\ & VCC))
-- \cnt[3]~24\ = CARRY((cnt(3) & !\cnt[2]~22\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(3),
	datad => VCC,
	cin => \cnt[2]~22\,
	combout => \cnt[3]~23_combout\,
	cout => \cnt[3]~24\);

-- Location: FF_X25_Y23_N19
\cnt[3]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[3]~23_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(3));

-- Location: LCCOMB_X25_Y23_N20
\cnt[4]~25\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[4]~25_combout\ = (cnt(4) & (!\cnt[3]~24\)) # (!cnt(4) & ((\cnt[3]~24\) # (GND)))
-- \cnt[4]~26\ = CARRY((!\cnt[3]~24\) # (!cnt(4)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(4),
	datad => VCC,
	cin => \cnt[3]~24\,
	combout => \cnt[4]~25_combout\,
	cout => \cnt[4]~26\);

-- Location: FF_X25_Y23_N21
\cnt[4]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[4]~25_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(4));

-- Location: LCCOMB_X25_Y23_N24
\cnt[6]~29\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[6]~29_combout\ = (cnt(6) & (!\cnt[5]~28\)) # (!cnt(6) & ((\cnt[5]~28\) # (GND)))
-- \cnt[6]~30\ = CARRY((!\cnt[5]~28\) # (!cnt(6)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(6),
	datad => VCC,
	cin => \cnt[5]~28\,
	combout => \cnt[6]~29_combout\,
	cout => \cnt[6]~30\);

-- Location: FF_X25_Y23_N25
\cnt[6]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[6]~29_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(6));

-- Location: LCCOMB_X25_Y23_N26
\cnt[7]~31\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[7]~31_combout\ = (cnt(7) & (\cnt[6]~30\ $ (GND))) # (!cnt(7) & (!\cnt[6]~30\ & VCC))
-- \cnt[7]~32\ = CARRY((cnt(7) & !\cnt[6]~30\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(7),
	datad => VCC,
	cin => \cnt[6]~30\,
	combout => \cnt[7]~31_combout\,
	cout => \cnt[7]~32\);

-- Location: LCCOMB_X25_Y23_N28
\cnt[8]~33\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[8]~33_combout\ = (cnt(8) & (!\cnt[7]~32\)) # (!cnt(8) & ((\cnt[7]~32\) # (GND)))
-- \cnt[8]~34\ = CARRY((!\cnt[7]~32\) # (!cnt(8)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(8),
	datad => VCC,
	cin => \cnt[7]~32\,
	combout => \cnt[8]~33_combout\,
	cout => \cnt[8]~34\);

-- Location: FF_X25_Y23_N29
\cnt[8]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[8]~33_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(8));

-- Location: LCCOMB_X25_Y23_N30
\cnt[9]~35\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[9]~35_combout\ = (cnt(9) & (\cnt[8]~34\ $ (GND))) # (!cnt(9) & (!\cnt[8]~34\ & VCC))
-- \cnt[9]~36\ = CARRY((cnt(9) & !\cnt[8]~34\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => cnt(9),
	datad => VCC,
	cin => \cnt[8]~34\,
	combout => \cnt[9]~35_combout\,
	cout => \cnt[9]~36\);

-- Location: LCCOMB_X25_Y22_N0
\cnt[10]~37\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[10]~37_combout\ = (cnt(10) & (!\cnt[9]~36\)) # (!cnt(10) & ((\cnt[9]~36\) # (GND)))
-- \cnt[10]~38\ = CARRY((!\cnt[9]~36\) # (!cnt(10)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(10),
	datad => VCC,
	cin => \cnt[9]~36\,
	combout => \cnt[10]~37_combout\,
	cout => \cnt[10]~38\);

-- Location: FF_X25_Y22_N1
\cnt[10]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[10]~37_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(10));

-- Location: LCCOMB_X25_Y22_N2
\cnt[11]~39\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[11]~39_combout\ = (cnt(11) & (\cnt[10]~38\ $ (GND))) # (!cnt(11) & (!\cnt[10]~38\ & VCC))
-- \cnt[11]~40\ = CARRY((cnt(11) & !\cnt[10]~38\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(11),
	datad => VCC,
	cin => \cnt[10]~38\,
	combout => \cnt[11]~39_combout\,
	cout => \cnt[11]~40\);

-- Location: FF_X25_Y22_N3
\cnt[11]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[11]~39_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(11));

-- Location: LCCOMB_X25_Y22_N4
\cnt[12]~41\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[12]~41_combout\ = (cnt(12) & (!\cnt[11]~40\)) # (!cnt(12) & ((\cnt[11]~40\) # (GND)))
-- \cnt[12]~42\ = CARRY((!\cnt[11]~40\) # (!cnt(12)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(12),
	datad => VCC,
	cin => \cnt[11]~40\,
	combout => \cnt[12]~41_combout\,
	cout => \cnt[12]~42\);

-- Location: FF_X25_Y22_N5
\cnt[12]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[12]~41_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(12));

-- Location: LCCOMB_X25_Y22_N8
\cnt[14]~45\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[14]~45_combout\ = (cnt(14) & (!\cnt[13]~44\)) # (!cnt(14) & ((\cnt[13]~44\) # (GND)))
-- \cnt[14]~46\ = CARRY((!\cnt[13]~44\) # (!cnt(14)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(14),
	datad => VCC,
	cin => \cnt[13]~44\,
	combout => \cnt[14]~45_combout\,
	cout => \cnt[14]~46\);

-- Location: FF_X25_Y22_N9
\cnt[14]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[14]~45_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(14));

-- Location: LCCOMB_X25_Y22_N14
\cnt[17]~51\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[17]~51_combout\ = (cnt(17) & (\cnt[16]~50\ $ (GND))) # (!cnt(17) & (!\cnt[16]~50\ & VCC))
-- \cnt[17]~52\ = CARRY((cnt(17) & !\cnt[16]~50\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(17),
	datad => VCC,
	cin => \cnt[16]~50\,
	combout => \cnt[17]~51_combout\,
	cout => \cnt[17]~52\);

-- Location: FF_X25_Y22_N15
\cnt[17]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[17]~51_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(17));

-- Location: LCCOMB_X25_Y22_N16
\cnt[18]~53\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[18]~53_combout\ = (cnt(18) & (!\cnt[17]~52\)) # (!cnt(18) & ((\cnt[17]~52\) # (GND)))
-- \cnt[18]~54\ = CARRY((!\cnt[17]~52\) # (!cnt(18)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => cnt(18),
	datad => VCC,
	cin => \cnt[17]~52\,
	combout => \cnt[18]~53_combout\,
	cout => \cnt[18]~54\);

-- Location: FF_X25_Y22_N17
\cnt[18]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[18]~53_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(18));

-- Location: LCCOMB_X25_Y22_N18
\cnt[19]~55\ : cycloneive_lcell_comb
-- Equation(s):
-- \cnt[19]~55_combout\ = \cnt[18]~54\ $ (!cnt(19))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111000000001111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datad => cnt(19),
	cin => \cnt[18]~54\,
	combout => \cnt[19]~55_combout\);

-- Location: FF_X25_Y22_N19
\cnt[19]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[19]~55_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(19));

-- Location: LCCOMB_X25_Y22_N28
\Equal0~0\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~0_combout\ = (cnt(16) & (cnt(19) & (cnt(17) & cnt(18))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(16),
	datab => cnt(19),
	datac => cnt(17),
	datad => cnt(18),
	combout => \Equal0~0_combout\);

-- Location: FF_X25_Y23_N31
\cnt[9]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[9]~35_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(9));

-- Location: LCCOMB_X24_Y22_N8
\Equal0~4\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~4_combout\ = (cnt(11) & (cnt(10) & (cnt(9) & cnt(8))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(11),
	datab => cnt(10),
	datac => cnt(9),
	datad => cnt(8),
	combout => \Equal0~4_combout\);

-- Location: FF_X25_Y23_N27
\cnt[7]\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \cnt[7]~31_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => cnt(7));

-- Location: LCCOMB_X25_Y23_N8
\Equal0~2\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~2_combout\ = (cnt(7) & cnt(6))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => cnt(7),
	datad => cnt(6),
	combout => \Equal0~2_combout\);

-- Location: LCCOMB_X25_Y23_N6
\Equal0~1\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~1_combout\ = (cnt(0) & (cnt(3) & (cnt(1) & cnt(2))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(0),
	datab => cnt(3),
	datac => cnt(1),
	datad => cnt(2),
	combout => \Equal0~1_combout\);

-- Location: LCCOMB_X25_Y23_N2
\Equal0~3\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~3_combout\ = (cnt(5) & (cnt(4) & (\Equal0~2_combout\ & \Equal0~1_combout\)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => cnt(5),
	datab => cnt(4),
	datac => \Equal0~2_combout\,
	datad => \Equal0~1_combout\,
	combout => \Equal0~3_combout\);

-- Location: LCCOMB_X24_Y22_N18
\Equal0~6\ : cycloneive_lcell_comb
-- Equation(s):
-- \Equal0~6_combout\ = (\Equal0~5_combout\ & (\Equal0~0_combout\ & (\Equal0~4_combout\ & \Equal0~3_combout\)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Equal0~5_combout\,
	datab => \Equal0~0_combout\,
	datac => \Equal0~4_combout\,
	datad => \Equal0~3_combout\,
	combout => \Equal0~6_combout\);

-- Location: FF_X24_Y22_N3
\led[1]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[1]~1_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[1]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N0
\led[2]~reg0feeder\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[2]~reg0feeder_combout\ = \led[1]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[1]~reg0_q\,
	combout => \led[2]~reg0feeder_combout\);

-- Location: FF_X24_Y22_N1
\led[2]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[2]~reg0feeder_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[2]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N26
\led[3]~2\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[3]~2_combout\ = !\led[2]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[2]~reg0_q\,
	combout => \led[3]~2_combout\);

-- Location: FF_X24_Y22_N27
\led[3]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[3]~2_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[3]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N28
\led[4]~reg0feeder\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[4]~reg0feeder_combout\ = \led[3]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111000011110000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => \led[3]~reg0_q\,
	combout => \led[4]~reg0feeder_combout\);

-- Location: FF_X24_Y22_N29
\led[4]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[4]~reg0feeder_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[4]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N10
\led[5]~reg0feeder\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[5]~reg0feeder_combout\ = \led[4]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[4]~reg0_q\,
	combout => \led[5]~reg0feeder_combout\);

-- Location: FF_X24_Y22_N11
\led[5]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[5]~reg0feeder_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[5]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N16
\led[6]~3\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[6]~3_combout\ = !\led[5]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[5]~reg0_q\,
	combout => \led[6]~3_combout\);

-- Location: FF_X24_Y22_N17
\led[6]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[6]~3_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[6]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N6
\led[7]~reg0feeder\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[7]~reg0feeder_combout\ = \led[6]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[6]~reg0_q\,
	combout => \led[7]~reg0feeder_combout\);

-- Location: FF_X24_Y22_N7
\led[7]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[7]~reg0feeder_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[7]~reg0_q\);

-- Location: LCCOMB_X24_Y22_N12
\led[0]~0\ : cycloneive_lcell_comb
-- Equation(s):
-- \led[0]~0_combout\ = !\led[7]~reg0_q\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \led[7]~reg0_q\,
	combout => \led[0]~0_combout\);

-- Location: FF_X24_Y22_N13
\led[0]~reg0\ : dffeas
-- pragma translate_off
GENERIC MAP (
	is_wysiwyg => "true",
	power_up => "low")
-- pragma translate_on
PORT MAP (
	clk => \ext_clk_25m~inputclkctrl_outclk\,
	d => \led[0]~0_combout\,
	clrn => \ext_rst_n~inputclkctrl_outclk\,
	ena => \Equal0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	q => \led[0]~reg0_q\);

ww_led(0) <= \led[0]~output_o\;

ww_led(1) <= \led[1]~output_o\;

ww_led(2) <= \led[2]~output_o\;

ww_led(3) <= \led[3]~output_o\;

ww_led(4) <= \led[4]~output_o\;

ww_led(5) <= \led[5]~output_o\;

ww_led(6) <= \led[6]~output_o\;

ww_led(7) <= \led[7]~output_o\;
END structure;



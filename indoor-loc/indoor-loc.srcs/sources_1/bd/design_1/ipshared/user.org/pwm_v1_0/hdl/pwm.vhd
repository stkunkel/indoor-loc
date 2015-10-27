----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    08:51:38 08/01/2014 
-- Design Name: 
-- Module Name:    pwm - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity pwm is
	generic ( 
		clkScale: integer := 100; -- clk enable ratio
		resolution: integer := 8 -- bits
	);

	Port ( 
		clk : in  STD_LOGIC;  
		rst : in  STD_LOGIC;
		-- update : in  STD_LOGIC; -- we update at end of PWM period
		val : IN  std_logic_vector(resolution - 1 downto 0);
		steps: in std_logic_vector(resolution - 1 downto 0);
		pulse: out std_logic;
		done: out std_logic
	);
end pwm;


architecture Behavioral of pwm is

signal enable: STD_LOGIC;
signal  val_i :   std_logic_vector(resolution - 1 downto 0) := (others => '0');

begin

	-- prescale 
	process
		variable cnt: integer range 0 to clkScale := 0;
	begin
		wait until rising_edge(clk);
		if rst = '1' then
			enable <= '0';
			cnt := 0;
		elsif cnt < clkScale - 1 then
			enable <= '0';
			cnt := cnt + 1;
		else
			enable <= '1';
			cnt := 0;
		end if;
	end process;

	-- capture value
	-- we capture the value in the driver process
--	process
--	begin
--		wait until rising_edge(clk);
--		if update = '1' then 
--			val_i <= val;
--		end if;
--	end process;
	
	-- driver
	process
		variable cnt: integer range 0 to 2**resolution := 0;
	begin
		wait until rising_edge(clk);
		if rst = '1' then
			done <= '0';
			pulse <= '0';
			cnt := 0;
			val_i <= (others => '0'); 
		elsif enable = '1' then 
			if cnt < to_integer(unsigned(val_i))  then 
				done <= '0';
				pulse <= '1';
				cnt := cnt + 1;
			elsif cnt < to_integer(unsigned(steps)) - 1 then 
				done <= '0';
				pulse <= '0';
				cnt := cnt + 1;
			else 
				done <= '1';
				cnt := 0;
				val_i <= val; -- capture (new) value
			end if;
		end if;
	end process;

	
end Behavioral;


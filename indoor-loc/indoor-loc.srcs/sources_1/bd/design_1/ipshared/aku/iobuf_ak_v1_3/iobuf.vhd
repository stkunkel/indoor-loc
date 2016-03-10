----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/15/2016 03:05:35 PM
-- Design Name: 
-- Module Name: iobuf - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity iobuf_ak is
    Port ( din : in STD_LOGIC;
           tin : in STD_LOGIC;
           dout : out STD_LOGIC;
           pin : inout STD_LOGIC);
end iobuf_ak;

architecture Behavioral of iobuf_ak is

begin

IOBUF_inst : IOBUF
port map (
O => dout,
-- Buffer output
IO => pin,
-- Buffer inout port (connect directly to top-level port)
I => din,
-- Buffer input
T => tin
-- 3-state enable input, high=input, low=output
);

--    pin <= din when tin = '0' else 'Z';
--    dout <= pin;

end Behavioral;

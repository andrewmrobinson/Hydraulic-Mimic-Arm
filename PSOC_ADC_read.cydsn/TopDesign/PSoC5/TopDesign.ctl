-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-030(050) kit.
-- =============================================================================

-- === UART ===
attribute port_location of Rx(0) : label is "PORT(0,5)";
attribute port_location of Tx(0) : label is "PORT(0,4)";

-- === USER SWITCH ===
attribute port_location of SW2(0) : label is "PORT(6,1)";

-- === LED ===
attribute port_location of LED(0) : label is "PORT(6,2)"; 

-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-042-BLE and CY8CKIT-042-BLE-A kit.
-- =============================================================================

-- === UART ===
attribute port_location of Rx(0) : label is "PORT(1,4)";
attribute port_location of Tx(0) : label is "PORT(1,5)";

-- === USER SWITCH ===
attribute port_location of SW2(0) : label is "PORT(2,7)";

-- === LED ===
attribute port_location of LED(0) : label is "PORT(2,6)"; 


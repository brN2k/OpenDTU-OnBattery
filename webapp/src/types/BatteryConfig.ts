export interface BatteryZendureConfig {
    device_type: number;
    device_id: string;
    polling_interval: number;
    soc_min: number;
    soc_max: number;
    bypass_mode: number;
    max_output: number;
    auto_shutdown: boolean;
    output_limit: number;
    output_control: number;
    output_limit_day: number;
    output_limit_night: number;
    sunrise_offset: number;
    sunset_offset: number;
    charge_through_enable: boolean;
    charge_through_interval: number;
    buzzer_enable: boolean;
    control_mode: number;
    charge_through_keep_minutes: number;
    connection_type: number;
    server: string;
    port: number;
    client_id: string;
    app_key: string;
    secret: string;
    battery_protection_enable: boolean;
    battery_protection_hysteresis: number;
}

export interface BatteryMqttConfig {
    soc_topic: string;
    soc_json_path: string;
    soc_backup_topic: string;
    soc_backup_json_path: string;
    voltage_topic: string;
    voltage_json_path: string;
    voltage_unit: number;
    current_topic: string;
    current_json_path: string;
    current_unit: number;
    discharge_current_limit_topic: string;
    discharge_current_limit_json_path: string;
    discharge_current_limit_unit: number;
    charge_current_limit_topic: string;
    charge_current_limit_json_path: string;
    charge_current_limit_unit: number;
    solar_input_power_topic: string;
    solar_input_power_json_path: string;
    solar_input_power_backup_topic: string;
    solar_input_power_backup_json_path: string;
    lowest_cell_voltage_topic: string;
    lowest_cell_voltage_json_path: string;
    lowest_cell_voltage_backup_topic: string;
    lowest_cell_voltage_backup_json_path: string;
    highest_cell_voltage_topic: string;
    highest_cell_voltage_json_path: string;
    highest_cell_voltage_backup_topic: string;
    highest_cell_voltage_backup_json_path: string;
    topic_timeout: number;
}

export interface BatterySerialConfig {
    interface: number;
    polling_interval: number;
}

export interface BatteryConfig {
    enabled: boolean;
    provider: number;
    serial: BatterySerialConfig;
    mqtt: BatteryMqttConfig;
    zendure: BatteryZendureConfig;
    enable_discharge_current_limit: boolean;
    discharge_current_limit: number;
    discharge_current_limit_below_soc: number;
    discharge_current_limit_below_voltage: number;
    use_battery_reported_discharge_current_limit: boolean;
    enable_charge_current_limit: boolean;
    min_charge_current_limit: number;
    max_charge_current_limit: number;
    charge_current_limit_below_soc: number;
    charge_current_limit_below_voltage: number;
    use_battery_reported_charge_current_limit: boolean;
    keep_at_soc_enabled: boolean;
    keep_at_soc: number;
    keep_at_soc_behavior: number;
    keep_at_soc_top_off_target_voltage: number;
    keep_at_soc_top_off_hard_dump_voltage: number;
    keep_at_soc_top_off_hard_dump_release_margin: number;
    keep_at_soc_top_off_efficiency: number;
    keep_at_soc_top_off_charge_power: number;
    low_cell_voltage_protection_enabled: boolean;
    low_cell_voltage_threshold: number;
    low_cell_voltage_protection_mode: number;
    low_cell_voltage_recovery_margin: number;
    low_cell_voltage_solar_hold_efficiency: number;
}

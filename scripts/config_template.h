#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Controllino.h>
#include "wrapper.h"

// MQTT Configuration
#define MQTT_SERVER "{{ mqtt.server }}"
#define MQTT_SERVER_PORT {{ mqtt.port }}
#define MQTT_USERNAME "{{ mqtt.username }}"
#define MQTT_PASSWORD "{{ mqtt.password }}"
#define MQTT_CLIENT_ID String("{{ mqtt.client_id }}")
#define MQTT_DEVICE_NAME "{{ mqtt.device_name | default('Controllino PLC') }}"
#define MQTT_DISCOVERY_PREFIX "{{ mqtt.discovery_prefix | default('homeassistant') }}"
#define MQTT_STATE_ON_PAYLOAD "ON"
#define MQTT_STATE_OFF_PAYLOAD "OFF"
#define MQTT_CONTROLLINO_CONFIG_TOPIC String(MQTT_DISCOVERY_PREFIX + String("/status/{{ mqtt.client_id }}")).c_str()
#define MQTT_BROKER_TIMEOUT 5000

// Network Configuration
byte ETH_MAC[] = { {{ network.mac | join(', ') }} };
IPAddress ETH_IP({{ network.ip | replace('.', ', ') }});
IPAddress ETH_GATEWAY({{ network.gateway | replace('.', ', ') }});
IPAddress ETH_SUBNET({{ network.subnet | replace('.', ', ') }});

// Global Objects
{% for button in buttons -%}
Button {{ button.id }}({{ button.pin }});
{% endfor %}

{% for light in lights -%}
Light {{ light.id }}({{ light.pin }});
{% endfor %}

// Shared Button Arrays for M-to-1 mappings
{% for mapping in mappings if mapping.buttons|length > 1 -%}
Button* buttons_{{ mapping.id }}[] = { 
    {%- for b_id in mapping.buttons -%}
    &{{ b_id }}{{ ", " if not loop.last }}
    {%- endfor -%}
};
{% endfor %}

// Triggers
Trigger triggers[] = {
    {%- for mapping in mappings %}
    {%- if mapping.buttons|length > 1 %}
    Trigger(buttons_{{ mapping.id }}, {{ mapping.buttons|length }}, &{{ mapping.light }}, "{{ mapping.name }}", "{{ mapping.id }}", "{{ mapping.type | default('light') }}")
    {%- elif mapping.buttons|length == 1 %}
    Trigger(&{{ mapping.buttons[0] }}, &{{ mapping.light }}, "{{ mapping.name }}", "{{ mapping.id }}", "{{ mapping.type | default('light') }}")
    {%- else %}
    Trigger((Button*)NULL, &{{ mapping.light }}, "{{ mapping.name }}", "{{ mapping.id }}", "{{ mapping.type | default('light') }}")
    {%- endif %}{{ "," if not loop.last }}
    {%- endfor %}
};

// All buttons for scanning
Button* all_buttons[] = {
    {%- for button in buttons %}
    &{{ button.id }}{{ "," if not loop.last }}
    {%- endfor %}
};

#endif

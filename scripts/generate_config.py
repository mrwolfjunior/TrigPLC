import yaml
import jinja2
import os
import sys

def generate_config(yaml_file, template_file, output_file):
    if not os.path.exists(yaml_file):
        print(f"Error: {yaml_file} not found.")
        return

    with open(yaml_file, 'r') as f:
        config_data = yaml.safe_load(f)

    with open(template_file, 'r') as f:
        template_str = f.read()

    template = jinja2.Template(template_str)
    output = template.render(config_data)

    with open(output_file, 'w') as f:
        f.write(output)
    
    print(f"Successfully generated {output_file}")

if __name__ == "__main__":
    # Default paths relative to project root
    yaml_path = "config.yaml"
    template_path = "scripts/config_template.h"
    output_path = "src/config.h"

    if len(sys.argv) > 1:
        yaml_path = sys.argv[1]
    
    generate_config(yaml_path, template_path, output_path)

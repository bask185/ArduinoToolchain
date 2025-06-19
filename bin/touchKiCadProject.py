#!/usr/bin/env python

import os

def create_kicad_pro(project_name):
    return f"""{{
  "board": {{
    "design_settings": {{
      "defaults": {{}},
      "diff_pair_dimensions": [],
      "drc_exclusions": [],
      "rules": {{}},
      "track_widths": [],
      "via_dimensions": []
    }}
  }},
  "boards": [],
  "libraries": {{
    "pinned_footprint_libs": [],
    "pinned_symbol_libs": []
  }},
  "meta": {{
    "filename": "{project_name}.kicad_pro",
    "version": 1
  }},
  "net_settings": {{
    "classes": [],
    "meta": {{
      "version": 0
    }}
  }},
  "pcbnew": {{
    "page_layout_descr_file": ""
  }},
  "sheets": [],
  "text_variables": {{}}
}}"""

def create_kicad_sch():
    return """(kicad_sch (version 20231120) (generator "eeschema") (generator_version "8.0")
  (paper "A4")
  (lib_symbols)
  (symbol_instances)
)"""

def create_kicad_pcb():
    return """(kicad_pcb (version 20240108) (generator "pcbnew") (generator_version "8.0")
)"""

def create_kicad_project(project_name):
    print(f"Creating project folder: {project_name}")
    os.makedirs(project_name, exist_ok=True)
    
    # File paths
    pro_file_path = os.path.join(project_name, f"{project_name}.kicad_pro")
    sch_file_path = os.path.join(project_name, f"{project_name}.kicad_sch")
    pcb_file_path = os.path.join(project_name, f"{project_name}.kicad_pcb")
    
    print("Writing .kicad_pro file")
    with open(pro_file_path, 'w') as pro_file:
        pro_file.write(create_kicad_pro(project_name))
    
    print("Writing .kicad_sch file")
    with open(sch_file_path, 'w') as sch_file:
        sch_file.write(create_kicad_sch())
    
    print("Writing .kicad_pcb file")
    with open(pcb_file_path, 'w') as pcb_file:
        pcb_file.write(create_kicad_pcb())
    
    print(f"KiCad project '{project_name}' created successfully.")

#if __name__ == "__main__":
project_name = input("Enter the project name: ")
print("name entered!")
create_kicad_project(project_name)

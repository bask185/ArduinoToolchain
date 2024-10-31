#!/usr/bin/env python

import os

def create_kicad_project():
    # Prompt voor de projectnaam
    project_name = input("Voer de naam van het KiCad project in: ")

    # Maak de projectdirectory aan
    project_dir = os.path.join('.', project_name)
    
    # Controleer of de directory al bestaat
    if not os.path.exists(project_dir):
        os.makedirs(project_dir)
        print(f"Directory '{project_dir}' gemaakt.")
    else:
        print(f"Directory '{project_dir}' bestaat al.")

    # Bestandspaden voor de .kicad_pro, .kicad_sch en .kicad_pcb bestanden
    project_file = os.path.join(project_dir, f"{project_name}.kicad_pro")
    schematic_file = os.path.join(project_dir, f"{project_name}.kicad_sch")
    pcb_file = os.path.join(project_dir, f"{project_name}.kicad_pcb")

    # Inhoud voor het .kicad_pro bestand
    kicad_pro_content = f'''{{
  "board": {{
    "3dviewports": [],
    "design_settings": {{
      "defaults": {{}},
      "diff_pair_dimensions": [],
      "drc_exclusions": [],
      "rules": {{}},
      "track_widths": [],
      "via_dimensions": []
    }},
    "ipc2581": {{
      "dist": "",
      "distpn": "",
      "internal_id": "",
      "mfg": "",
      "mpn": ""
    }},
    "layer_presets": [],
    "viewports": []
  }},
  "boards": [],
  "cvpcb": {{
    "equivalence_files": []
  }},
  "libraries": {{
    "pinned_footprint_libs": [],
    "pinned_symbol_libs": []
  }},
  "meta": {{
    "filename": "{project_name}.kicad_pro",
    "version": 1
  }},
  "net_settings": {{
    "classes": [
      {{
        "bus_width": 12,
        "clearance": 0.2,
        "diff_pair_gap": 0.25,
        "diff_pair_via_gap": 0.25,
        "diff_pair_width": 0.2,
        "line_style": 0,
        "microvia_diameter": 0.3,
        "microvia_drill": 0.1,
        "name": "Default",
        "pcb_color": "rgba(0, 0, 0, 0.000)",
        "schematic_color": "rgba(0, 0, 0, 0.000)",
        "track_width": 0.2,
        "via_diameter": 0.6,
        "via_drill": 0.3,
        "wire_width": 6
      }}
    ],
    "meta": {{
      "version": 3
    }},
    "net_colors": null,
    "netclass_assignments": null,
    "netclass_patterns": []
  }},
  "pcbnew": {{
    "last_paths": {{
      "gencad": "",
      "idf": "",
      "netlist": "",
      "plot": "",
      "pos_files": "",
      "specctra_dsn": "",
      "step": "",
      "svg": "",
      "vrml": ""
    }},
    "page_layout_descr_file": ""
  }},
  "schematic": {{
    "legacy_lib_dir": "",
    "legacy_lib_list": []
  }},
  "sheets": [],
  "text_variables": {{}}
}}'''

    # Inhoud voor het .kicad_sch bestand
    kicad_sch_content = '''(kicad_sch (version 20231120) (generator "eeschema") (generator_version "8.0")
  (paper "A4")
  (lib_symbols)
  (symbol_instances)
)
'''

    # Inhoud voor het .kicad_pcb bestand
    kicad_pcb_content = '''(kicad_pcb (version 20240108) (generator "pcbnew") (generator_version "8.0")
)
'''

    # Schrijf de inhoud naar de respectieve bestanden
    with open(project_file, 'w') as f:
        f.write(kicad_pro_content)
    print(f"KiCad projectbestand '{project_file}' aangemaakt.")

    with open(schematic_file, 'w') as f:
        f.write(kicad_sch_content)
    print(f"Schemabestand '{schematic_file}' aangemaakt.")

    with open(pcb_file, 'w') as f:
        f.write(kicad_pcb_content)
    print(f"PCB-bestand '{pcb_file}' aangemaakt.")

# Voer het script uit
create_kicad_project()

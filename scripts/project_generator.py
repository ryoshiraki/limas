import os
import glob
import json
import shutil
import sys
import subprocess
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox
from tkinter import Tk
import configparser

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.dirname(script_dir)

root = Tk()
root.title("Project Generator")
root.geometry("200x200+500+500")
root.withdraw()

project_parent_dir = filedialog.askdirectory(
    title = "Choose Project Directory",
    initialdir = f"{root_dir}/apps",
    parent = root
)
if not project_parent_dir:
    sys.exit()
print(f"Project directory is {project_parent_dir}")

project_name = simpledialog.askstring("",
                                      "Enter Project Name",
                                      parent = root)
if not project_name:
    sys.exit()
print(f"Project name is {project_name}")
project_dir = os.path.join(project_parent_dir, project_name)

rel_path_from_project_to_root = os.path.relpath(root_dir, start=project_dir)

#use_pytohn = messagebox.askquestion(title="Python", message="Use Python?") == 'yes'

#addon_cmakes = []
#while True:
#  use_addon = messagebox.askquestion(title="Addon", message="Use Addon?") == 'yes'
#  if not use_addon:
#    break
#
#  addon_dir = filedialog.askdirectory(
#  title = "Choose Addon Directory",
#  initialdir = f"{root_dir}/addons",
#  parent = root)
#  
#  abs_addon_cmakes = glob.glob(os.path.join(addon_dir, '*' + ".cmake"))
#  if len(abs_addon_cmakes) > 0:
#      addon_cmakes += [os.path.relpath(path, start=root_dir) for path in abs_addon_cmakes]

template_path = os.path.join(script_dir, "templates/app_template")
template_workspace_file_name = [f for f in os.listdir(template_path) if f.endswith(".code-workspace")][0]
workspace_file_name = f"{project_name}.code-workspace"

def ignore_directories(dir, filenames):
    return ['build']

shutil.copytree(template_path, project_dir, ignore=ignore_directories)
os.rename(
    os.path.join(project_dir, template_workspace_file_name),
    os.path.join(project_dir, workspace_file_name)
)

with open(os.path.join(project_dir, workspace_file_name), 'r+') as f:
    data = json.load(f)

    for folder in data['folders']:
        if folder['name'] == "template":
            folder['name'] = project_name
        elif folder['name'] == "limas":
            folder['path'] = os.path.join(rel_path_from_project_to_root, "libs/limas")
        # elif folder['name'] == "addons":
        #     folder['path'] = os.path.join(rel_path_from_project_to_root, "addons")
        # elif folder['name'] == "resources":
        #     folder['path'] = os.path.join(rel_path_from_project_to_root, "resources")

    f.seek(0)
    json.dump(data, f, indent=4)
    f.truncate()

with open(os.path.join(project_dir, ".vscode/c_cpp_properties.json"), 'r+') as f:
    data = json.load(f)

    library_path = os.path.join(rel_path_from_project_to_root, "libs/limas/**")
    for configuration in data["configurations"]:
        configuration["includePath"].append(library_path)
    
    f.seek(0)
    json.dump(data, f, indent=4)
    f.truncate()

with open(os.path.join(project_dir, "CMakeLists.txt"), "w") as f:
    f.write("cmake_minimum_required(VERSION 3.5)\n\n")
    f.write(f"project({project_name} CXX OBJCXX)\n")
    f.write("set(FRAMEWORK_PATH ${PROJECT_SOURCE_DIR}/" + os.path.relpath(root_dir, project_dir) + ")\n")
    # f.write("set(FRAMEWORK_PATH \"" + root_dir + "\")\n")
    f.write("add_definitions(-DFRAMEWORK_PATH=\"${FRAMEWORK_PATH}\")\n")
    f.write("include(${FRAMEWORK_PATH}/scripts/limas.cmake)\n\n")
    
#    for addon in addon_cmakes:
#        f.write("include(${ROOT_DIR}/"+f"{addon})\n")
    
#    if use_pytohn:
#        conf = configparser.ConfigParser()
#        conf.read("./conf.ini")
#        python_header = conf.get('Python', 'header')
#        python_libray = conf.get('Python', 'library')
#
#        f.write(f"set(PYTHON_HEADER \"{python_header}\")\n")
#        f.write(f"set(PYTHON_LIBRARY \"{python_libray}\")\n")
#        f.write("target_include_directories(${PROJECT_NAME} PRIVATE ${PYTHON_HEADER})\n")
#        f.write("target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARY})\n")

subprocess.run(["code", f"{project_dir}/{workspace_file_name}"])

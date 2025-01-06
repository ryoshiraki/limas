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

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.dirname(script_dir)

root = Tk()
root.title("Project Generator")
root.geometry("200x200+500+500")
root.withdraw()

# Select the project directory
project_parent_dir = filedialog.askdirectory(
    title="Choose Project Directory",
    initialdir=f"{root_dir}/apps",
    parent=root
)
if not project_parent_dir:
    sys.exit()
print(f"Project directory is {project_parent_dir}")

# Enter the project name
project_name = simpledialog.askstring("",
                                      "Enter Project Name",
                                      parent=root)
if not project_name:
    sys.exit()
print(f"Project name is {project_name}")
project_dir = os.path.join(project_parent_dir, project_name)

rel_path_from_project_to_root = os.path.relpath(root_dir, start=project_dir)

# Select the template directory using a dialog
template_path = filedialog.askdirectory(
    title="Choose Template Directory",
    initialdir=os.path.join(script_dir, "templates"),
    parent=root
)
if not template_path or not os.path.isdir(template_path):
    messagebox.showerror("Error", "No valid template folder selected.")
    sys.exit()
print(f"Selected template: {template_path}")

# Get the workspace file from the template
template_workspace_file_name = [f for f in os.listdir(template_path) if f.endswith(".code-workspace")]
if not template_workspace_file_name:
    messagebox.showerror("Error", "No .code-workspace file found in template.")
    sys.exit()
template_workspace_file_name = template_workspace_file_name[0]
workspace_file_name = f"{project_name}.code-workspace"

# Copy template files to the project directory
def ignore_directories(dir, filenames):
    return ['build']

shutil.copytree(template_path, project_dir, ignore=ignore_directories)
os.rename(
    os.path.join(project_dir, template_workspace_file_name),
    os.path.join(project_dir, workspace_file_name)
)

# Update the workspace file
with open(os.path.join(project_dir, workspace_file_name), 'r+') as f:
    data = json.load(f)

    for folder in data['folders']:
        if folder['name'] == "template":
            folder['name'] = project_name
        elif folder['name'] == "limas":
            folder['path'] = os.path.join(rel_path_from_project_to_root, "libs/limas")

    f.seek(0)
    json.dump(data, f, indent=4)
    f.truncate()

# Update c_cpp_properties.json
with open(os.path.join(project_dir, ".vscode/c_cpp_properties.json"), 'r+') as f:
    data = json.load(f)

    library_path = os.path.join(rel_path_from_project_to_root, "libs/limas/**")
    for configuration in data["configurations"]:
        configuration["includePath"].append(library_path)
    
    f.seek(0)
    json.dump(data, f, indent=4)
    f.truncate()

# Create CMakeLists.txt
with open(os.path.join(project_dir, "CMakeLists.txt"), "w") as f:
    f.write("cmake_minimum_required(VERSION 3.5)\n\n")
    f.write(f"project({project_name} CXX OBJCXX)\n")
    f.write("set(FRAMEWORK_PATH ${PROJECT_SOURCE_DIR}/" + os.path.relpath(root_dir, project_dir) + ")\n")
    f.write("add_definitions(-DFRAMEWORK_PATH=\"${FRAMEWORK_PATH}\")\n")
    f.write("include(${FRAMEWORK_PATH}/scripts/limas.cmake)\n\n")
    
# Open the project in Visual Studio Code
subprocess.run(["code", f"{project_dir}/{workspace_file_name}"])

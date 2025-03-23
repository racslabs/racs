# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'zform'
copyright = '2025, AUXTS Labs Ltd.'
author = 'AUXTS Labs'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

#extensions = ['breathe', 'sphinx.ext.intersphinx']
extensions = ['sphinx.ext.intersphinx']
templates_path = ['_templates']
exclude_patterns = []
#highlight_language = 'c++'

# breathe_projects = {project: '../build/xml'}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_logo = 'white-logo.svg'
html_theme = 'piccolo_theme'
html_static_path = ['_static']
html_css_files = [
    'custom.css',
]

intersphinx_mapping = {'python': ('https://docs.python.org/3', None)}

#breathe_default_project = project

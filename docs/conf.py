# Sphinx configuration for SqC Developer Guide

project = 'SqC'
author = 'BISSELL Homecare, Inc.'
copyright = '2026, BISSELL Homecare, Inc.'

extensions = []

templates_path = []
exclude_patterns = ['screenshots/README.md']

# -- HTML output (sphinx-rtd-theme) --
html_theme = 'sphinx_rtd_theme'
html_static_path = []

# -- LaTeX / PDF output --
latex_elements = {
    'papersize': 'letterpaper',
    'pointsize': '10pt',
    'preamble': r'''
\usepackage{enumitem}
\setlistdepth{9}
''',
}

latex_documents = [
    ('index', 'sqc-developer-guide.tex', 'SqC Developer Guide',
     'BISSELL Homecare, Inc.', 'manual'),
]

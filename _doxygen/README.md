# Doxygen Documentation for the C Telemetry SDK

The API documentation for the C Telemetry SDK is generated using Doxygen.  The public
header, newrelic-telemetry-sdk.h, is annotated with Doxygen commands and the documentation
available at [github.io/newrelic-c-telemetry-sdk](https://newrelic.github.io/newrelic-c-telemetry-sdk/index.html)
is the output of invoking the `doxygen` command from the `_doxygen` directory.

Since, by default, Doxygen looks a bit like a website from 1998, the C SDK
uses Bootstrap for styling, along with some HTML header and footer files 
augmented with guidance from the OSS
[doxygen-bootstrapped](https://github.com/Velron/doxygen-bootstrapped/).

Here are some highlights of how to configure and style the Doxygen documentation
for the C Telemetry SDK.

## Configuration

The Doxyfile contains the Doxygen configuration, including a list of files
that must be used as input as well as the file that is used for the main
page.

This is the set of input file configuration values:

```
EXAMPLE_PATH            = ../examples/
HTML_HEADER             = header.html
HTML_FOOTER             = footer.html
HTML_STYLESHEET         = customdoxygen.css
INPUT                   = ../include/newrelic-telemetry-sdk.h ../README.md 
USE_MDFILE_AS_MAINPAGE  = ../README.md
```

These are the other relevant configuration values that do not have
default values:

```
FULL_PATH_NAMES        = NO
HTML_DYNAMIC_SECTIONS  = NO
HTML_OUTPUT            = ../html
HTML_TIMESTAMP         = YES
OPTIMIZE_OUTPUT_FOR_C  = YES
PROJECT_NAME           = "New Relic C Telemetry SDK"
SHOW_INCLUDE_FILES     = NO
TYPEDEF_HIDES_STRUCT   = YES
GENERATE_LATEX         = NO
```

The Doxyfile describes each of these settings. Note that any changes to 
`FULL_PATH_NAMES` or `TYPEDEF_HIDES_STRUCT` may change the structure of 
some of the Doxygen URLs and potentially break links from docs.newrelic.com
to gihub.io.

## Updating

The latest Doxygen files -- including `Doxyfile`, `header.html`, `footer.html`, 
and `customdoxygen.css`-- were generated with version 1.8.15. This section
details the steps necessary if one must recreate these files with a newer
version of Doxygen.

Doxygen creates a new template `Doxyfile` with the following command:

```
doxygen -g
```

Take care to update this new template configuration file with the 
settings described above.

Doxygen also allows for the customization of its HTML output.  Begin 
by invoking a one-time doxygen command to create a default `header.html`, 
`footer.html` and `customdoxygen.css`; these files may have changed 
with a new version of Doxygen.

```
doxygen -w html header.html footer.html customdoxygen.css
```

If these files already exist, Doxygen moves the existing file to a `.bak`
version.  That is, after invoking the command, the old `header.html` is
`header.html.bak` and the newly generated default is `header.html`.

The augmented `header.html` and `footer.html` files required for the C SDK 
differs from their default in the following ways:

### Include Open Sans and bootstrap in header.html

```
<link href="https://fonts.googleapis.com/css?family=Open+Sans" rel="stylesheet">
<link rel="stylesheet" type='text/css' href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css">
```

### Remove the tabs.css file from the header

Remove the following line from header

```
<link href="$relpath^tabs.css" rel="stylesheet" type="text/css"/>
```

The tabs.css file is generated automatically by Doxygen.  It styles the top navbar 
but includes some dated design choices.  Copy the default `tab.css` content into 
`customdoxygen.css` and remove the use of `Lucida` and text shadow.

### Create bootstrap-style navbar in header.html

```
<body>
    <nav class="navbar navbar-default" role="navigation">
        <div class="container">
            <div class="navbar-header">
                <a class="navbar-brand">$projectname $projectnumber</a>
            </div>
        </div>
    </nav>
    <div id="top"><!-- do not remove this div, it is closed by doxygen! -->
        <div class="content" id="content">
            <div class="container">
                <div class="row">
                    <div class="col-sm-12 panel " style="padding-bottom: 15px;">
                        <div style="margin-bottom: 15px;">
```

### Close the bootstrap-style navbar in footer.html

```
</div>
</div>
</div>
</div>
</div>
<!--BEGIN !GENERATE_TREEVIEW-->
```

Performing a diff between `header.html` and `header.html.bak` offers more details
on where to make these changes.  Same goes for `footer.html` and `footer.html.bak`.

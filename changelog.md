# Changelog

## [v1.0.0]

### Features

* Add/Remove Layer
* Add/Remove Edge
* Add/Remove Port
* Find Layer
* Layer properties
* Edge properties
* Relayout
* Open/Save

## [v1.0.1]

### Changes

* Updated icon

## [v1.1.0]

### Features

* Properties panel is now displaying port properties
* Added multiple selection option
* Extended find panel options
* Added validation panel
* Added Copy/Paste commands

### Changes

* Fixed crash on big models
* Fixed redo command
* Removed edge arrows

## [v1.1.1]

### Features

* Properties panel is now displaying port dimensions
* Validation message for edges that are connecting two ports with different dimensions
* "Const" Layers are disappearing when they are not selected or not connected to a selected node
* Ports that are connected to "Const" layer are colored in gray.

### Changes

* Properties panel does no longer expand layer properties when more than two layers are selected
* Properties panel has "goto" buttons when displaying edge properties that allows to navigate canvas to connecting layers
* Fixed crash when undo and redo after deleting a port
* When creating graph layout the first row is dedicated to layers of type: "Parameter" and the last row is dedicated to layers of type "Result"
* Layers of type "Const" are treated as part of the layer they are connected to.

## [v1.1.2]

### Changes

* Fixed crash on exiting application
* Improved layout. All models in a row are now aligned at the bottom. Previously they were aligned at the top. Previously it looked weird when a row contained layers with attached const layers and layers without attached const layers.
* Improved layout speed. Which means models will open faster.
* Improved frame rate. Edges that connect layers that are not visible are no longer being drawn. 


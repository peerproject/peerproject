Schema XSD files provide for search selection, metadata fields, download group types, etc. 
Schema XML files provide for translation and presentation of XSD.

Library Organizer file/folder inclusion itself remains hard-coded.
Filetype .ext handling is in core LibraryBuilderInternals.cpp, and various Library Builder Plugins.
Default folders are in LibraryFolder, AlbumFolder, etc.  (Initially translated by Schema at first run)

SchemaDescriptor URLs (limewire.com/shareaza.com) are universal names, and cannot easily be changed.
Schema conflict "mapping" is in SchemaMappings.xml and Schema.cpp, etc.

The exact filename from .xsd is used to detect related resources: (.xml/.ico)
Library icons may be reassigned in xml.  Simply overwrite existing icons to skin. (Not upgrade-safe)
".Safe" variant icons are used when original cannot be loaded. (Windows2000/etc.)

"typeFilter" extensions in xml are used for download group default filters.
"Database" folder is used for convenience in xml translation. (Likely outdated)


Please note many aspects are mutually dependant in various places,
any functional change must often be mirrored elsewhere.  (xsd/xml/code)

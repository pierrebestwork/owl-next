Copyright OWLNext community
ObjectWindows (C) 1998-2007

Title: VCL2OWL Example

Keywords: VCL

This example shows how to link a VCL application with a OWLNext dialog.
It's based on the original document by Kent Reisdorph.
The idea is to use a 'unit' for each OWL Dialog, when calling the OWL dialog
create a Window Object by aliasing the windows handle from the VCL form
which will be used as parent window.

Rules:
- Keep in mind that VCL apps don't use 'enums as ints', this option is the
default for others compilers. Set the project default as VCL requires and
redefine in each OWL 'unit'.
- Data alignment for classic OWL apps was 1 byte. VCL apps use 4 bytes
(BCB3/4/5/6) or 8 bytes (BCB2006/2007). Transfer buffers must always be 
byte-packed. You can ensure that your transfer buffers are byte-packed by 
including <pshpack1.h> before and <poppack.h> after the buffer definition.




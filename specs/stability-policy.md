# Stability Policy

## Extensions

### Extension Stablity

Extensions, including the Main ISA Specification, are considered to be stable in the first version of the clever ISA published that includes them. Stable extensions are guaranteed not to be changed in future versions, except to fix unclear wording, defects, or specification holes. 
In fixing such bugs, care will be taken to make the least disruptive changes, and both machine implementors and supervisor developers may submit informative comments on the relevent issues filed against the Working Repository. 

Futher, it is guaranteed that, subject to the "Permitted Changes" section, the behaviour as prescribed by a published extension will not be modified by other extensions.

Extensions that are to be included in future publications will have a tracking issue filed with sufficient notice to the publication of that version and Reviewers will be notified to review the extension. 
Additionally, community members, including but not limited to machine implementors, toolchain vendors, and supervisor developers, may comment on the proposed extension or may file issues related to the proposed extension up until publication.

### Future Extensions

Extensions may be proposed for inclusion in the specification by Pull Request to the Working Repository. Reviewers will be notified to review the proposed extension. After the Proposal is Reviewed, it will be opened for Finalization, which closes 7 days after the it begins and the last blocking issue filed against it is closed. 
The Finalized Extension will be included in the Working Repository, but is not yet set for inclusion in a published version. A separate issue will be filed in accordance with "Extension Stability" to propose stabilization and publication. 

### Permitted Changes

With the exception of necessary changes to correct defects, unclear wording, or spec holes, only a subset of changes to stablized extensions are permitted. 
Changes that affect supervisors and applications compiled against previous versions of the specification or with a particular extension disabled are not permitted. In particular:
- Subsequent extensions cannot change the prescribed behaviour of any operation except for behaviour in the case of a prescribed exception, unless the supervisor or program opts-in to that change.
- Subsequent extensions cannot change the prescribed floating-point exceptions of any instruction, unless the supervisor or program opts-in to that change.
- Subsequent extensions cannot change the prescribed exceptions of any instruction performed from program execution mode, unless the supervisor opts-in to that change, but the prescribed exceptions of instructions performed from supervisor mode may be modified. 
    - In particular, the introduction of a new program instruction (opcode<0x800) is permitted with a supervisor opt-in. 
- Subsequent extensions cannot change the behaviour of accessing registers 63 and 255. 
- Subsequent extensions cannot change the behaviour of instructions 0x000 and 0xfff.
- Subsequent extensions cannot change the definition of io addresses prescribed by this specification.

Additionally, the following behaviours are considered reserved to the implementation, and may not be changed by extensions:
- The behaviour and existance of instructions 0xfe0-0xfff, and instructions subsequently defined as "Machine Specific"
- The behaviour of accessing registers 143, and 148-154, and other registers subequently defined as "Machine Specific"


Notwithstanding the above, the following kinds of extensions are permitted:
- Virtualization and the introduction of hypervisors that may manage and are permitted to trap or otherwise handle behaviour indicated by extensions, provided that these behaviours are opted-into by the particular hypervisor,
- Modifications to correct defects, unclear wording, or holes in the specification (where the result of some operation is not prescribed by the specification, and not explicitly indicated as undefined).
- Introduction of instructions that trigger different exceptions from UND, even in program execution mode. 

Behaviour which is not considered stable:
- The order which checks that result in exceptions are performed, unless a stable order is explicitly prescribed.
- The behaviour of undefined supervisor instructions (opcode >=0x800) executed from supervisor mode.
- Any behaviour explicitly documented as undefined. 

Such behaviours may be modified at will by the specification or by particular machine vendors, without any kind of opt-in from the supervisor.


## Changes to the Stability Policy

Changes to this Stability Policy may be made in a future revision, provided any behaviour previously considered stable is not affected. 


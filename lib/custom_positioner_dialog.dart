import 'package:flutter/material.dart';
import 'flutter_view_positioner.dart';

Future<Map<String, dynamic>?> customPositionerDialog(
  BuildContext context,
  Map<String, dynamic> settings,
) async {
  return await showDialog(
      barrierDismissible: true,
      context: context,
      builder: (BuildContext ctx) {
        // Dialog data
        // String name = settings['name'].toString();
        // int weight = settings['weight'] as int;
        // int height = settings['height'] as int;
        // int age = settings['age'] as int;
        // bool pushups = settings['pushups'].toString() == 'true';
        String name = settings['name'].toString();
        Offset offset = settings['offset'] as Offset;
        final List<String> anchor = FlutterViewPositionerAnchor.values
            .map((e) => e.toString().split('.').last)
            .toList();
        bool slideX = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.slideX);
        bool slideY = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.slideY);
        bool flipX = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.flipX);
        bool flipY = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.flipY);
        bool resizeX = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.resizeX);
        bool resizeY = (settings['constraintAdjustments']
                as Set<FlutterViewPositionerConstraintAdjustment>)
            .contains(FlutterViewPositionerConstraintAdjustment.resizeY);
        String parentAnchor =
            (settings['parentAnchor'] as FlutterViewPositionerAnchor)
                .toString()
                .split('.')
                .last;
        String childAnchor =
            (settings['childAnchor'] as FlutterViewPositionerAnchor)
                .toString()
                .split('.')
                .last;

        return StatefulBuilder(
            builder: (BuildContext ctx, StateSetter setState) {
          return SimpleDialog(
            contentPadding: const EdgeInsets.all(8),
            titlePadding: const EdgeInsets.fromLTRB(24, 20, 24, 0),
            title: const Center(
              child: Text('Custom Positioner'),
            ),
            children: [
              // Padding(
              //   padding: const EdgeInsets.all(8),
              //   child: TextFormField(
              //       initialValue: name,
              //       decoration: const InputDecoration(
              //           icon: Icon(Icons.label), labelText: 'Name'),
              //       onChanged: (String value) => setState(() => name = value),),
              // ),
              // Padding(
              //   padding: const EdgeInsets.all(8),
              //   child: TextFormField(
              //       initialValue: weight.toString(),
              //       decoration: const InputDecoration(
              //           icon: Icon(Icons.assessment), labelText: 'Weight'),
              //       onChanged: (String value) =>
              //           setState(() => weight = int.parse(value),),),
              // ),
              Expanded(
                child: ListTile(
                  title: const Text('Parent Anchor'),
                  subtitle: DropdownButton(
                    items: anchor.map<DropdownMenuItem<String>>((String value) {
                      return DropdownMenuItem<String>(
                        value: value,
                        child: Text(value),
                      );
                    }).toList(),
                    value: parentAnchor,
                    isExpanded: true,
                    focusColor: Colors.transparent,
                    onChanged: (String? value) {
                      setState(() {
                        parentAnchor = value!;
                      });
                    },
                  ),
                ),
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Child Anchor'),
                  subtitle: DropdownButton(
                    items: anchor.map<DropdownMenuItem<String>>((String value) {
                      return DropdownMenuItem<String>(
                        value: value,
                        child: Text(value),
                      );
                    }).toList(),
                    value: childAnchor,
                    isExpanded: true,
                    focusColor: Colors.transparent,
                    onChanged: (String? value) {
                      setState(() {
                        childAnchor = value!;
                      });
                    },
                  ),
                ),
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Offset'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: offset.dx.toString(),
                          decoration: const InputDecoration(
                            // icon: Icon(null),
                            labelText: 'X',
                          ),
                          onChanged: (String value) => setState(
                            () => offset =
                                Offset(double.tryParse(value) ?? 0, offset.dy),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: offset.dy.toString(),
                          decoration: const InputDecoration(
                            // icon: Icon(null),
                            labelText: 'Y',
                          ),
                          onChanged: (String value) => setState(
                            () => offset =
                                Offset(offset.dx, double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              // Padding(
              //   padding: const EdgeInsets.all(8),
              //   child: Text('Age $age'),
              // ),
              // Slider(
              //     value: age.toDouble(),
              //     min: 0,
              //     max: 150,
              //     divisions: 5,
              //     label: age.toString(),
              //     onChanged: (double value) {
              //       setState(() {
              //         age = value.round();
              //       });
              //     }),
              const SizedBox(
                height: 8,
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Constraint Adjustments'),
                  subtitle: Column(
                    children: [
                      const SizedBox(
                        height: 16,
                      ),
                      Row(
                        children: [
                          Expanded(
                            child: Table(
                                defaultVerticalAlignment:
                                    TableCellVerticalAlignment.middle,
                                children: [
                                  const TableRow(children: [
                                    TableCell(
                                      child: Text(''),
                                    ),
                                    TableCell(
                                      child: Center(
                                        child: Text('X'),
                                      ),
                                    ),
                                    TableCell(
                                      child: Center(
                                        child: Text('Y'),
                                      ),
                                    ),
                                  ]),
                                  TableRow(children: [
                                    const TableCell(
                                      child: Text('Slide'),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: slideX,
                                        onChanged: (bool? value) =>
                                            setState(() => slideX = value!),
                                      ),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: slideY,
                                        onChanged: (bool? value) =>
                                            setState(() => slideY = value!),
                                      ),
                                    ),
                                  ]),
                                  TableRow(children: [
                                    const TableCell(
                                      child: Text('Flip'),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: flipX,
                                        onChanged: (bool? value) =>
                                            setState(() => flipX = value!),
                                      ),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: flipY,
                                        onChanged: (bool? value) =>
                                            setState(() => flipY = value!),
                                      ),
                                    ),
                                  ]),
                                  TableRow(children: [
                                    const TableCell(
                                      child: Text('Resize'),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: resizeX,
                                        onChanged: (bool? value) =>
                                            setState(() => resizeX = value!),
                                      ),
                                    ),
                                    TableCell(
                                      child: Checkbox(
                                        value: resizeY,
                                        onChanged: (bool? value) =>
                                            setState(() => resizeY = value!),
                                      ),
                                    ),
                                  ]),
                                ]),
                          ),
                          // Checkbox(
                          //     value: pushups,
                          //     onChanged: (bool? value) =>
                          //         setState(() => pushups = value!),),
                          // const Text('Push ups'),
                          // const SizedBox(
                          //   width: 20,
                          // ),
                          // Checkbox(
                          //     value: pushups,
                          //     onChanged: (bool? value) =>
                          //         setState(() => pushups = value!),),
                          // const Text('Push ups'),
                          // const SizedBox(
                          //   width: 20,
                          // ),
                          // Checkbox(
                          //     value: pushups,
                          //     onChanged: (bool? value) =>
                          //         setState(() => pushups = value!),),
                          // const Text('Push ups'),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 4,
              ),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                child: TextButton(
                  onPressed: () {
                    Set<FlutterViewPositionerConstraintAdjustment>
                        constraintAdjustments = {};
                    if (slideX) {
                      constraintAdjustments.add(
                          FlutterViewPositionerConstraintAdjustment.slideX);
                    }
                    if (slideY) {
                      constraintAdjustments.add(
                          FlutterViewPositionerConstraintAdjustment.slideY);
                    }
                    if (flipX) {
                      constraintAdjustments
                          .add(FlutterViewPositionerConstraintAdjustment.flipX);
                    }
                    if (flipY) {
                      constraintAdjustments
                          .add(FlutterViewPositionerConstraintAdjustment.flipY);
                    }
                    if (resizeX) {
                      constraintAdjustments.add(
                          FlutterViewPositionerConstraintAdjustment.resizeX);
                    }
                    if (resizeY) {
                      constraintAdjustments.add(
                          FlutterViewPositionerConstraintAdjustment.resizeY);
                    }
                    Navigator.of(context, rootNavigator: true)
                        .pop(<String, dynamic>{
                      // 'name': name,
                      // 'weight': weight,
                      // 'height': height,
                      // 'age': age,
                      // 'pushups': pushups,
                      'name': name,
                      'parentAnchor':
                          FlutterViewPositionerAnchor.values.firstWhere(
                        (e) =>
                            e.toString() ==
                            'FlutterViewPositionerAnchor.$parentAnchor',
                        orElse: () => FlutterViewPositionerAnchor.left,
                      ),
                      'childAnchor':
                          FlutterViewPositionerAnchor.values.firstWhere(
                        (e) =>
                            e.toString() ==
                            'FlutterViewPositionerAnchor.$childAnchor',
                        orElse: () => FlutterViewPositionerAnchor.left,
                      ),
                      'offset': offset,
                      'constraintAdjustments': constraintAdjustments,
                    });
                  },
                  child: const Text('Apply'),
                ),
              ),
              const SizedBox(
                height: 8,
              ),

              // GestureDetector(
              //   onTap: () {
              //     Set<FlutterViewPositionerConstraintAdjustment>
              //         constraintAdjustments = {};
              //     if (slideX) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.slideX);
              //     }
              //     if (slideY) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.slideY);
              //     }
              //     if (flipX) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.flipX);
              //     }
              //     if (flipY) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.flipY);
              //     }
              //     if (resizeX) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.resizeX);
              //     }
              //     if (resizeY) {
              //       constraintAdjustments
              //           .add(FlutterViewPositionerConstraintAdjustment.resizeY);
              //     }
              //     Navigator.of(context, rootNavigator: true)
              //         .pop(<String, dynamic>{
              //       // 'name': name,
              //       // 'weight': weight,
              //       // 'height': height,
              //       // 'age': age,
              //       // 'pushups': pushups,
              //       'parentAnchor':
              //           FlutterViewPositionerAnchor.values.firstWhere(
              //         (e) =>
              //             e.toString() ==
              //             'FlutterViewPositionerAnchor.$parentAnchor',
              //         orElse: () => FlutterViewPositionerAnchor.left,
              //       ),
              //       'childAnchor':
              //           FlutterViewPositionerAnchor.values.firstWhere(
              //         (e) =>
              //             e.toString() ==
              //             'FlutterViewPositionerAnchor.$childAnchor',
              //         orElse: () => FlutterViewPositionerAnchor.left,
              //       ),
              //       'offset': offset,
              //       'constraintAdjustments': constraintAdjustments,
              //     });
              //   },
              //   child: Container(
              //     padding: const EdgeInsets.all(16),
              //     // decoration: BoxDecoration(
              //     //   border: Border.all(
              //     //     width: 2,
              //     //   ),
              //     //   borderRadius: BorderRadius.circular(4),
              //     // ),
              //     child: const Center(
              //       child: Text('Apply'),
              //     ),
              //   ),
              // ),
            ],
          );
        });
      });
}

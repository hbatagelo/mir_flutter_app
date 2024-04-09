import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'flutter_view_positioner.dart';
import 'custom_positioner_dialog.dart';
import 'window_settings_dialog.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      title: 'Mir Window Test',
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({Key? key}) : super(key: key);

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  final windowChannel = const MethodChannel('io.mir-server/window');

  Map<String, dynamic> windowSettings = {
    'regularSize': const Size(400, 400),
    'floatingRegularSize': const Size(300, 300),
    'dialogSize': const Size(200, 200),
    'satelliteSize': const Size(150, 300),
    'anchorRect': const Rect.fromLTWH(0, 0, 400, 400),
  };

  int positionerIndex = 0;
  List<Map<String, dynamic>> positionerSettings = [
    // Left
    <String, dynamic>{
      'name': 'Left',
      'parentAnchor': FlutterViewPositionerAnchor.left,
      'childAnchor': FlutterViewPositionerAnchor.right,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Right
    <String, dynamic>{
      'name': 'Right',
      'parentAnchor': FlutterViewPositionerAnchor.right,
      'childAnchor': FlutterViewPositionerAnchor.left,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Bottom Left
    <String, dynamic>{
      'name': 'Bottom Left',
      'parentAnchor': FlutterViewPositionerAnchor.bottomLeft,
      'childAnchor': FlutterViewPositionerAnchor.topRight,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Bottom
    <String, dynamic>{
      'name': 'Bottom',
      'parentAnchor': FlutterViewPositionerAnchor.bottom,
      'childAnchor': FlutterViewPositionerAnchor.top,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Bottom Right
    <String, dynamic>{
      'name': 'Bottom Right',
      'parentAnchor': FlutterViewPositionerAnchor.bottomRight,
      'childAnchor': FlutterViewPositionerAnchor.topLeft,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Center
    <String, dynamic>{
      'name': 'Center',
      'parentAnchor': FlutterViewPositionerAnchor.center,
      'childAnchor': FlutterViewPositionerAnchor.center,
      'offset': const Offset(0, 0),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    },
    // Custom
    <String, dynamic>{
      'name': 'Custom',
      'parentAnchor': FlutterViewPositionerAnchor.left,
      'childAnchor': FlutterViewPositionerAnchor.right,
      'offset': const Offset(0, 50),
      'constraintAdjustments': <FlutterViewPositionerConstraintAdjustment>{
        FlutterViewPositionerConstraintAdjustment.slideX,
        FlutterViewPositionerConstraintAdjustment.slideY,
      }
    }
  ];

  List<Map<String, dynamic>> windows = [];
  int selectedRowIndex = -1;

  @override
  void initState() {
    super.initState();
    windowChannel.setMethodCallHandler(_methodCallHandler);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Mir Window Test'),
      ),
      body: Column(
        children: [
          Expanded(
            flex: 8,
            child: Container(),
          ),
          Expanded(
            flex: 82,
            child: ListView(
              padding: const EdgeInsets.symmetric(horizontal: 50.0),
              children: [
                Row(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Expanded(
                      flex: 5,
                      child: Container(),
                    ),
                    Expanded(
                      flex: 30,
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        children: [
                          SizedBox(
                            width: 500,
                            height: 500,
                            child: SingleChildScrollView(
                              scrollDirection: Axis.vertical,
                              child: DataTable(
                                showBottomBorder: true,
                                onSelectAll: (selected) {
                                  setState(() {
                                    selectedRowIndex = -1;
                                  });
                                },
                                columns: const [
                                  DataColumn(
                                    label: SizedBox(
                                      width: 20,
                                      child: Text(
                                        'ID',
                                        style: TextStyle(
                                          fontSize: 16,
                                        ),
                                      ),
                                    ),
                                  ),
                                  DataColumn(
                                    label: SizedBox(
                                      width: 120,
                                      child: Text(
                                        'Type',
                                        style: TextStyle(
                                          fontSize: 16,
                                        ),
                                      ),
                                    ),
                                  ),
                                  DataColumn(
                                      label: SizedBox(
                                        width: 20,
                                        child: Text(''),
                                      ),
                                      numeric: true),
                                ],
                                rows: windows
                                    .asMap()
                                    .entries
                                    .map<DataRow>((entry) {
                                  final int index = entry.key;
                                  final Map<String, dynamic> window =
                                      entry.value;
                                  final int windowId = window['id'];
                                  final String type = window['type'];
                                  final bool isSelected =
                                      selectedRowIndex == index;

                                  return DataRow(
                                    color: MaterialStateColor.resolveWith(
                                        (states) {
                                      if (states
                                          .contains(MaterialState.selected)) {
                                        return Theme.of(context)
                                            .colorScheme
                                            .primary
                                            .withOpacity(0.08);
                                      }
                                      return Colors.transparent;
                                    }),
                                    selected: isSelected,
                                    onSelectChanged: (selected) {
                                      setState(() {
                                        if (selected != null) {
                                          selectedRowIndex =
                                              selected ? index : -1;
                                        }
                                      });
                                    },
                                    cells: [
                                      DataCell(
                                        Text('$windowId'),
                                      ),
                                      DataCell(
                                        Text(type),
                                      ),
                                      DataCell(
                                        IconButton(
                                          icon:
                                              const Icon(Icons.delete_outlined),
                                          onPressed: () {
                                            closeWindow(windowId);
                                            resetWindowId(windowId);
                                          },
                                        ),
                                      ),
                                    ],
                                  );
                                }).toList(),
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                    Expanded(
                      flex: 1,
                      child: Container(),
                    ),
                    Expanded(
                      flex: 30,
                      child: Column(
                        // mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        crossAxisAlignment: CrossAxisAlignment.stretch,
                        children: [
                          Card.outlined(
                            margin: const EdgeInsets.symmetric(horizontal: 100),
                            child: Padding(
                              padding: const EdgeInsets.fromLTRB(26, 0, 26, 12),
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.center,
                                children: [
                                  const Padding(
                                    padding:
                                        EdgeInsets.only(top: 16, bottom: 16),
                                    child: Text(
                                      'New Window',
                                      style: TextStyle(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16.0,
                                      ),
                                    ),
                                  ),
                                  Column(
                                    crossAxisAlignment:
                                        CrossAxisAlignment.stretch,
                                    children: [
                                      OutlinedButton(
                                        onPressed: () async {
                                          final windowId =
                                              await createRegularWindow(
                                                  windowSettings[
                                                      'regularSize']);
                                          await setWindowId(windowId);
                                          setState(() {
                                            selectedRowIndex =
                                                windows.indexWhere((window) =>
                                                    window['id'] == windowId);
                                          });
                                        },
                                        child: const Text('Regular'),
                                      ),
                                      const SizedBox(height: 16),
                                      OutlinedButton(
                                        onPressed: () async {
                                          final windowId =
                                              await createFloatingRegularWindow(
                                                  windowSettings[
                                                      'floatingRegularSize']);
                                          await setWindowId(windowId);
                                          setState(() {
                                            selectedRowIndex =
                                                windows.indexWhere((window) =>
                                                    window['id'] == windowId);
                                          });
                                        },
                                        child: const Text('Floating Regular'),
                                      ),
                                      const SizedBox(height: 16),
                                      OutlinedButton(
                                        onPressed: () async {
                                          final windowId =
                                              await createDialogWindow(
                                                  windowSettings['dialogSize'],
                                                  selectedRowIndex >= 0
                                                      ? windows[
                                                              selectedRowIndex]
                                                          ['id']
                                                      : null);
                                          await setWindowId(windowId);
                                        },
                                        child: Text(selectedRowIndex >= 0
                                            ? 'Dialog of ID ${windows[selectedRowIndex]['id']}'
                                            : 'Dialog'),
                                      ),
                                      const SizedBox(height: 16),
                                      OutlinedButton(
                                        onPressed: selectedRowIndex >= 0
                                            ? () async {
                                                final parentSize =
                                                    await getWindowSize(windows[
                                                            selectedRowIndex]
                                                        ['id']);
                                                final Rect clampedAnchorRect =
                                                    Rect.fromLTWH(
                                                        windowSettings[
                                                                'anchorRect']
                                                            .left
                                                            .clamp(
                                                                0,
                                                                parentSize
                                                                    .width),
                                                        windowSettings[
                                                                'anchorRect']
                                                            .top
                                                            .clamp(
                                                                0,
                                                                parentSize
                                                                    .height),
                                                        windowSettings[
                                                                'anchorRect']
                                                            .width
                                                            .clamp(
                                                                0,
                                                                parentSize
                                                                    .width),
                                                        windowSettings[
                                                                'anchorRect']
                                                            .height
                                                            .clamp(
                                                                0,
                                                                parentSize
                                                                    .height));
                                                final windowId =
                                                    await createSatelliteWindow(
                                                  windows[selectedRowIndex]
                                                      ['id'],
                                                  windowSettings[
                                                      'satelliteSize'],
                                                  clampedAnchorRect,
                                                  FlutterViewPositioner(
                                                    parentAnchor:
                                                        positionerSettings[
                                                                positionerIndex]
                                                            ['parentAnchor'],
                                                    childAnchor:
                                                        positionerSettings[
                                                                positionerIndex]
                                                            ['childAnchor'],
                                                    offset: positionerSettings[
                                                            positionerIndex]
                                                        ['offset'],
                                                    constraintAdjustment:
                                                        positionerSettings[
                                                                positionerIndex]
                                                            [
                                                            'constraintAdjustments'],
                                                  ),
                                                );
                                                await setWindowId(windowId);
                                                setState(() {
                                                  // Cycle through presets when the last one (Custom preset) is not selected
                                                  if (positionerIndex !=
                                                      positionerSettings
                                                              .length -
                                                          1) {
                                                    positionerIndex =
                                                        (positionerIndex + 1) %
                                                            (positionerSettings
                                                                    .length -
                                                                1);
                                                  }
                                                });
                                              }
                                            : null,
                                        child: Text(selectedRowIndex >= 0
                                            ? 'Satellite of ID ${windows[selectedRowIndex]['id']}'
                                            : 'Satellite'),
                                      ),
                                      const SizedBox(height: 14),
                                      Container(
                                        alignment: Alignment.bottomRight,
                                        child: TextButton(
                                          child: const Text('SETTINGS'),
                                          onPressed: () {
                                            windowSettingsDialog(
                                                    context, windowSettings)
                                                .then(
                                              (Map<String, dynamic>? settings) {
                                                setState(() {
                                                  if (settings != null) {
                                                    windowSettings = settings;
                                                  }
                                                });
                                              },
                                            );
                                          },
                                        ),
                                      ),
                                      const SizedBox(width: 8),
                                    ],
                                  ),
                                ],
                              ),
                            ),
                          ),
                          const SizedBox(height: 20),
                          Card.outlined(
                            margin: const EdgeInsets.symmetric(horizontal: 100),
                            child: Padding(
                              padding: const EdgeInsets.fromLTRB(20, 0, 12, 12),
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.center,
                                children: [
                                  const Padding(
                                    padding:
                                        EdgeInsets.only(top: 16, bottom: 6),
                                    child: Text(
                                      'Positioner',
                                      style: TextStyle(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16.0,
                                      ),
                                    ),
                                  ),
                                  ListTile(
                                    title: const Text('Preset'),
                                    subtitle: DropdownButton(
                                      items: positionerSettings
                                          .map((map) => map['name'] as String)
                                          .toList()
                                          .map<DropdownMenuItem<String>>(
                                              (String value) {
                                        return DropdownMenuItem<String>(
                                          value: value,
                                          child: Text(value),
                                        );
                                      }).toList(),
                                      value: positionerSettings
                                          .map((map) => map['name'] as String)
                                          .toList()[positionerIndex],
                                      isExpanded: true,
                                      focusColor: Colors.transparent,
                                      onChanged: (String? value) {
                                        setState(() {
                                          positionerIndex = positionerSettings
                                              .map((map) =>
                                                  map['name'] as String)
                                              .toList()
                                              .indexOf(value!);
                                        });
                                      },
                                    ),
                                  ),
                                  const SizedBox(height: 6),
                                  Container(
                                    alignment: Alignment.bottomRight,
                                    child: Padding(
                                      padding: const EdgeInsets.only(right: 8),
                                      child: TextButton(
                                        child: const Text('CUSTOM PRESET'),
                                        onPressed: () {
                                          customPositionerDialog(
                                                  context,
                                                  positionerSettings[
                                                      positionerSettings
                                                              .length -
                                                          1])
                                              .then(
                                            (Map<String, dynamic>? settings) {
                                              setState(() {
                                                if (settings != null) {
                                                  positionerSettings[
                                                      positionerSettings
                                                              .length -
                                                          1] = settings;
                                                  positionerIndex =
                                                      positionerSettings
                                                              .length -
                                                          1;
                                                }
                                              });
                                            },
                                          );
                                        },
                                      ),
                                    ),
                                  ),
                                  const SizedBox(width: 8),
                                ],
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                    Expanded(
                      flex: 2,
                      child: Container(),
                    ),
                  ],
                ),
              ],
            ),
          ),
          Expanded(
            flex: 10,
            child: Container(),
          ),
        ],
      ),
    );
  }

  Future<int> createRegularWindow(Size size) async {
    final id = await windowChannel
        .invokeMethod('createRegularWindow', [size.width, size.height]);
    return id;
  }

  Future<int> createFloatingRegularWindow(Size size) async {
    final id = await windowChannel
        .invokeMethod('createFloatingRegularWindow', [size.width, size.height]);
    return id;
  }

  Future<int> createSatelliteWindow(int parent, Size size, Rect anchorRect,
      FlutterViewPositioner positioner) async {
    int constraintAdjustmentBitmask = 0;
    for (var adjustment in positioner.constraintAdjustment) {
      constraintAdjustmentBitmask |= 1 << adjustment.index;
    }
    final id = await windowChannel.invokeMethod('createSatelliteWindow', [
      parent,
      size.width,
      size.height,
      anchorRect.left,
      anchorRect.top,
      anchorRect.width,
      anchorRect.height,
      positioner.parentAnchor.index,
      positioner.childAnchor.index,
      positioner.offset.dx,
      positioner.offset.dy,
      constraintAdjustmentBitmask
    ]);
    return id;
  }

  Future<int> createDialogWindow(Size size, int? parent) async {
    final int parentId = parent ?? -1;
    final int id = await windowChannel.invokeMethod(
        'createDialogWindow', [size.width, size.height, parentId]);
    return id;
  }

  void closeWindow(int windowId) {
    windowChannel.invokeMethod('closeWindow', [windowId]);
  }

  Future<void> setWindowId(int windowId) async {
    final windowType = await getWindowType(windowId);
    setState(() {
      windows.add({"id": windowId, "type": windowType});
    });
  }

  void resetWindowId(int windowId) {
    final index = windows.indexWhere((window) => window['id'] == windowId);
    final selectedWindowId =
        selectedRowIndex != -1 ? windows[selectedRowIndex]['id'] : -1;
    setState(() {
      windows.remove(windows[index]);
      if (selectedWindowId != -1) {
        selectedRowIndex =
            windows.indexWhere((window) => window['id'] == selectedWindowId);
      } else {
        selectedRowIndex = -1;
      }
    });
  }

  Future<void> _methodCallHandler(MethodCall call) async {
    switch (call.method) {
      case 'resetWindowId':
        resetWindowId(call.arguments);
        break;
    }
  }

  Future<String> getWindowType(int windowId) async {
    final windowType =
        await windowChannel.invokeMethod('getWindowType', [windowId]);
    return windowType;
  }

  Future<Size> getWindowSize(int windowId) async {
    final sizeMap =
        await windowChannel.invokeMapMethod('getWindowSize', [windowId]);
    return Size(sizeMap!['width'], sizeMap['height']);
  }
}

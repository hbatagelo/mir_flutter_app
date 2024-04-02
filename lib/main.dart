import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'flutter_view_positioner.dart';

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
  List<int> windows = [];

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
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            GestureDetector(
              onTap: () async {
                final id = await createRegularWindow(const Size(400, 400));
                setWindowId(id);
              },
              child: Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  border: Border.all(width: 2),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: const Text('Create Regular Window'),
              ),
            ),
            const SizedBox(height: 16),
            GestureDetector(
              onTap: () async {
                final id =
                    await createFloatingRegularWindow(const Size(300, 300));
                setWindowId(id);
              },
              child: Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  border: Border.all(width: 2),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: const Text('Create Floating Regular Window'),
              ),
            ),
            const SizedBox(height: 16),
            GestureDetector(
              onTap: () async {
                final id = await createDialogWindow(const Size(200, 200),
                    windows.isNotEmpty ? windows.last : null);
                setWindowId(id);
              },
              child: Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  border: Border.all(width: 2),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: const Text('Create Dialog Window'),
              ),
            ),
            const SizedBox(height: 16),
            GestureDetector(
              onTap: windows.isNotEmpty
                  ? () async {
                      const FlutterViewPositioner positioner =
                          FlutterViewPositioner(
                        parentAnchor: FlutterViewPositionerAnchor.left,
                        childAnchor: FlutterViewPositionerAnchor.left,
                        offset: Offset(0, 50),
                        constraintAdjustment: {
                          FlutterViewPositionerConstraintAdjustment.slideX,
                          FlutterViewPositionerConstraintAdjustment.slideY,
                        },
                      );
                      final id = await createSatelliteWindow(
                          windows.last,
                          const Size(200, 300),
                          const Rect.fromLTWH(0, 0, 400, 400),
                          positioner);
                      setWindowId(id);
                    }
                  : null,
              child: Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  border: Border.all(
                    width: 2,
                    color: windows.isNotEmpty ? Colors.black : Colors.grey,
                  ),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: Text(
                  'Create Satellite Window',
                  style: TextStyle(
                    color: windows.isNotEmpty
                        ? null
                        : Colors.grey, // Set text color to grey when disabled
                  ),
                ),
              ),
            ),
            const SizedBox(height: 16),
            GestureDetector(
              onTap: windows.isNotEmpty
                  ? () {
                      final id = windows.removeLast();
                      closeWindow(id);
                      resetWindowId(id);
                    }
                  : null,
              child: Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  border: Border.all(
                    width: 2,
                    color: windows.isNotEmpty ? Colors.black : Colors.grey,
                  ),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: Text(
                  'Close Window',
                  style: TextStyle(
                    color: windows.isNotEmpty
                        ? null
                        : Colors.grey, // Set text color to grey when disabled
                  ),
                ),
              ),
            ),
          ],
        ),
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

  void setWindowId(int windowId) {
    setState(() {
      windows.add(windowId);
    });
  }

  void resetWindowId(int windowId) {
    setState(() {
      windows.remove(windowId);
    });
  }

  Future<void> _methodCallHandler(MethodCall call) async {
    switch (call.method) {
      case 'resetWindowId':
        resetWindowId(call.arguments);
        break;
    }
  }
}

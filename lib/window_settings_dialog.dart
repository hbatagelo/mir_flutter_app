import 'package:flutter/material.dart';

Future<Map<String, dynamic>?> windowSettingsDialog(
  BuildContext context,
  Map<String, dynamic> settings,
) async {
  return await showDialog(
      barrierDismissible: true,
      context: context,
      builder: (BuildContext ctx) {
        Size regularSize = settings['regularSize'] as Size;
        Size floatingRegularSize = settings['floatingRegularSize'] as Size;
        Size dialogSize = settings['dialogSize'] as Size;
        Size satelliteSize = settings['satelliteSize'] as Size;
        Rect anchorRect = settings['anchorRect'] as Rect;

        return StatefulBuilder(
            builder: (BuildContext ctx, StateSetter setState) {
          return SimpleDialog(
            contentPadding: const EdgeInsets.all(8),
            titlePadding: const EdgeInsets.fromLTRB(24, 20, 24, 0),
            title: const Center(
              child: Text('Window Settings'),
            ),
            children: [
              Expanded(
                child: ListTile(
                  title: const Text('Regular'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: regularSize.width.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial width',
                          ),
                          onChanged: (String value) => setState(
                            () => regularSize = Size(
                                double.tryParse(value) ?? 0,
                                regularSize.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: regularSize.height.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial height',
                          ),
                          onChanged: (String value) => setState(
                            () => regularSize = Size(
                                regularSize.width, double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 8,
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Floating Regular'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: floatingRegularSize.width.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial width',
                          ),
                          onChanged: (String value) => setState(
                            () => floatingRegularSize = Size(
                                double.tryParse(value) ?? 0,
                                floatingRegularSize.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: floatingRegularSize.height.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial height',
                          ),
                          onChanged: (String value) => setState(
                            () => floatingRegularSize = Size(
                                floatingRegularSize.width,
                                double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 8,
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Dialog'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: dialogSize.width.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial width',
                          ),
                          onChanged: (String value) => setState(
                            () => dialogSize = Size(
                                double.tryParse(value) ?? 0, dialogSize.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: dialogSize.height.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial height',
                          ),
                          onChanged: (String value) => setState(
                            () => dialogSize = Size(
                                dialogSize.width, double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 8,
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Satellite'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: satelliteSize.width.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial width',
                          ),
                          onChanged: (String value) => setState(
                            () => satelliteSize = Size(
                                double.tryParse(value) ?? 0,
                                satelliteSize.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: satelliteSize.height.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Initial height',
                          ),
                          onChanged: (String value) => setState(
                            () => satelliteSize = Size(satelliteSize.width,
                                double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 8,
              ),
              Expanded(
                child: ListTile(
                  title: const Text('Anchor Rectangle'),
                  subtitle: Row(
                    children: [
                      Expanded(
                        child: TextFormField(
                          initialValue: anchorRect.left.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Left',
                          ),
                          onChanged: (String value) => setState(
                            () => anchorRect = Rect.fromLTWH(
                                double.tryParse(value) ?? 0,
                                anchorRect.top,
                                anchorRect.width,
                                anchorRect.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: anchorRect.top.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Top',
                          ),
                          onChanged: (String value) => setState(
                            () => anchorRect = Rect.fromLTWH(
                                anchorRect.left,
                                double.tryParse(value) ?? 0,
                                anchorRect.width,
                                anchorRect.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: anchorRect.width.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Width',
                          ),
                          onChanged: (String value) => setState(
                            () => anchorRect = Rect.fromLTWH(
                                anchorRect.left,
                                anchorRect.top,
                                double.tryParse(value) ?? 0,
                                anchorRect.height),
                          ),
                        ),
                      ),
                      const SizedBox(
                        width: 20,
                      ),
                      Expanded(
                        child: TextFormField(
                          initialValue: anchorRect.height.toString(),
                          decoration: const InputDecoration(
                            labelText: 'Height',
                          ),
                          onChanged: (String value) => setState(
                            () => anchorRect = Rect.fromLTWH(
                                anchorRect.left,
                                anchorRect.top,
                                anchorRect.width,
                                double.tryParse(value) ?? 0),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(
                height: 12,
              ),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 16),
                child: TextButton(
                  onPressed: () {
                    Navigator.of(context, rootNavigator: true)
                        .pop(<String, dynamic>{
                      'regularSize': regularSize,
                      'floatingRegularSize': floatingRegularSize,
                      'dialogSize': dialogSize,
                      'satelliteSize': satelliteSize,
                      'anchorRect': anchorRect,
                    });
                  },
                  child: const Text('Apply'),
                ),
              ),
              const SizedBox(
                height: 8,
              ),
            ],
          );
        });
      });
}

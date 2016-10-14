<?php

/**
 * @file
 * Bean tile container.
 */

class TilesBeanContainer extends TilesContainer {
  protected $container = 'bean_container';

  /**
   * Implements parent::getRegions().
   */
  public function getRegions() {
    // There's only one region for bean containers.
    return array(
      'content' => 'Content',
    );
  }

  /**
   * Implements parent::renderManifest().
   */
  protected function renderManifest($page) {
    $manifest = $this->getManifest();

    $layout = $this->getLayout($manifest->selector);

    // Clear out any current blocks in passed region.
    $layout->clearTiles($manifest->region);

    // Split blocks out by breakpoint.
    $blocks = array();
    foreach ($manifest->blocks as $block) {
      foreach ($block->breakpoints as $key => $width) {
        $new_block = clone $block;
        unset($new_block->breakpoints);
        $new_block->breakpoint = $key;
        $new_block->width = $width;
        $blocks[] = $new_block;
      }
    }

    // Add blocks back to layout.
    foreach ($blocks as $block) {
      $layout->addBlock($block);
    }

    // Load up bean so it can be passed to bean container plugin.
    $bean = bean_load_delta($manifest->selector);

    // Use the BeanContainer plugin to render layout, since bean containers can
    // have radically different output than normal tiles wrapping (e.g. tabbed
    // containers).
    $bean_container = bean_load_plugin_class('bean_container');
    $build = $bean_container->build($layout, $bean);

    print drupal_render($build['content']);
  }
}

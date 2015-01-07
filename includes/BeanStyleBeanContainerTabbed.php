<?php

/**
 * @file
 * Tabbed style for bean containers.
 */

class BeanStyleBeanContainerTabbed extends BeanStyleBeanContainer {
  /**
   * Implements parent::prepareView().
   */
  public function prepareView($build, $bean) {
    $build = parent::prepareView($build, $bean);

    $build['content'] = array(
      '#theme' => 'bean_container_tabs',
      '#children' => $this->items,
      '#parent' => $bean,
    );

    return $build;
  }

  /**
   * Implements parent::prepareItems().
   */
  public function prepareItems($build, $type) {
    parent::prepareItems($build, $type);

    // Store flat list of renderable blocks, since tabs only need widths.
    $this->items = $this->layout->getRenderBlocks('content');

    // For now, tabbed containers don't support block widths. It would be great
    // in the future if we would treat "rows" as tabs, so multiple blocks can
    // appear in a single tab.
    if ($this->items) {
      foreach (element_children($this->items) as $child) {
        $this->items[$child]['#block']->width = tiles_get_max_step();
      }
    }
  }
}

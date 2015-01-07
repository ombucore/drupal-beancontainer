<?php

/**
 * @file
 * Default style for bean containers.
 */

class BeanStyleBeanContainer extends BeanStyle {
  /**
   * Tiles layout object.
   *
   * @var TileLayout
   */
  protected $layout;

  /**
   * Implements parent::prepareView().
   */
  public function prepareView($build, $bean) {
    $build = parent::prepareView($build, $bean);

    // Use the tile container to properly wrap blocks within bean container.
    $container = tiles_get_container('bean_container');
    $container->buildPage($build, $this->layout);
    $container->wrapRegion($build['content']);

    return $build;
  }

  /**
   * Implements parent::prepareItems().
   */
  protected function prepareItems($build, $type) {
    $this->layout = $build['#layout'];
  }
}

<?php

/**
 * @file
 * Bean plugin object for bean container.
 */

class BeanContainer extends BeanPlugin {
  /**
   * Implements parent::view().
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    $container = tiles_get_container('bean_container');
    $layout = $container->getLayout($bean->delta);

    $content['bean'][$bean->delta]['children'] = $this->build($layout, $bean);

    // Add wrapper so tiles ordering works correctly.
    $content['bean']['#prefix'] = '<div class="content" data-name="content" data-type="bean_container" data-tiles-selector="' . $bean->delta . '">';
    $content['bean']['#suffix'] = '</div>';

    // Attach admin script to trigger active tab on reordering.
    if (tiles_access('move', 'bean', $bean->delta)) {
      $content['#attached']['js'][] = drupal_get_path('module', 'bean_container') . '/js/bean-container.js';
    }

    return $content;
  }

  /**
   * Build up render array for bean container.
   *
   * @param TileLayout $layout
   *   TileLayout object used to populate children blocks.
   * @param Bean $bean
   *   The bean object being viewed.
   *
   * @return array
   *   Build render array.
   */
  public function build($layout, $bean) {
    $build = array(
      '#layout' => $layout,
      'content' => array(),
    );

    // Don't call bean_style_view_alter(), since the render array currently is
    // outside of the bean render array. Instead use bean style api.
    $style = bean_style_bean_load_style($bean->bid);
    if ($style && $style->getStyle()) {
      $build = $style->getStyle()->prepareView($build, $bean);

      // Allow other modules to alter bean container styles.
      $context = array(
        'bean' => $bean,
        'style' => $style,
      );
      drupal_alter('bean_style_view', $build, $context);

      // Store bean style type with bean. This is used later to generate css class
      // on block wrapper.
      $bean->bean_style = $style->type;
    }

    return $build;
  }
}
